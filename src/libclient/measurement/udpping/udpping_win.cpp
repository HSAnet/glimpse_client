#include "udpping.h"

#include <time.h>
#include <Windows.h>
#include <mswsock.h>
#include <Mstcpip.h>
#include <pcap.h>

namespace
{

    bool getAddress(const QString &address, sockaddr_any *addr)
    {
        struct addrinfo hints;
        struct addrinfo *rp = NULL, *result = NULL;

        memset(&hints, 0, sizeof(hints));
        hints.ai_family = AF_INET;
        hints.ai_flags = AI_FQDN;

        if (getaddrinfo(address.toStdString().c_str(), NULL, &hints, &result))
        {
            return false;
        }

        for (rp = result; rp && rp->ai_family != AF_INET; rp = rp->ai_next)
        {
        }

        if (!rp)
        {
            rp = result;
        }

        memcpy(addr, rp->ai_addr, rp->ai_addrlen);

        freeaddrinfo(result);

        return true;
    }

    static PingProbe receiveLoop(pcap_t *capture, PingProbe probe, sockaddr_any destination)
    {
        struct ipAddress
        {
            u_char byte1;
            u_char byte2;
            u_char byte3;
            u_char byte4;
        };

        int res;
        char sourceAddress[16] = "";
        char destinationAddress[16] = "";
        const u_char *data;
        pcap_pkthdr *header;
        ipAddress *source;
        quint8 *icmpType;
        quint8 *icmpCode;
        quint8 *ipProto;
        PingProbe newProbe;

        memcpy(&newProbe, &probe, sizeof(newProbe));

        for (;;)
        {
            res = pcap_next_ex(capture, &header, &data);

            ipProto = (quint8 *) (data + 23);
            source = (ipAddress *) (data + 26);
            icmpType = (quint8 *) (data + 34);
            icmpCode = (quint8 *) (data + 35);

            switch (res)
            {
            case 0:
                // timed out
                newProbe.sendTime = 0;
                newProbe.recvTime = 0;
                goto exit;
            case -1:
                // error indication
                goto error;
            default:
                // packets received
                // TODO: ensure the packets are really ours by checking them

                // source address of the response packet
                sprintf(sourceAddress, "%d.%d.%d.%d", source->byte1, source->byte2, source->byte3, source->byte4);
                // destination of request packet
                strncpy(destinationAddress, inet_ntoa(destination.sin.sin_addr), sizeof(destinationAddress));

                if (*ipProto == 17)
                {
                    // UDP request
                    newProbe.sendTime = header->ts.tv_sec * 1e6 + header->ts.tv_usec;
                } else if (*ipProto == 1)
                {
                    // ICMP response
                    if (*icmpCode == 3 && *icmpType == 3)
                    {
                        // destination and port unreachable: this was a successful ping
                        if (strncmp(sourceAddress, destinationAddress, 16) == 0)
                        {
                            newProbe.recvTime = header->ts.tv_sec * 1e6 + header->ts.tv_usec;
                            newProbe.response = DESTINATION_UNREACHABLE;
                            goto exit;
                        }
                    } else if (*icmpCode == 0 && *icmpType == 11)
                    {
                        /*
                         * TTL exceeded
                         *
                         * Let's missuse source and sourceAddress for the destination of the original IP header.
                         */
                        source = (ipAddress *) (data + 76);
                        sprintf(sourceAddress, "%d.%d.%d.%d", source->byte1, source->byte2, source->byte3, source->byte4);

                        if (strncmp(sourceAddress, destinationAddress, 16) == 0)
                        {
                            newProbe.recvTime = header->ts.tv_sec * 1e6 + header->ts.tv_usec;
                            newProbe.response = TTL_EXCEEDED;
                            goto exit;
                        }
                    } else
                    {
                        /*
                         * An unhandled ICMP packet has been captured. We need to check this and
                         * handle it if needed.
                         */
                        newProbe.icmpType = *icmpType;
                        newProbe.icmpCode = *icmpCode;
                        newProbe.response = UNHANDLED_ICMP;
                        goto exit;
                    }
                } else
                {
                    /*
                     * This else-branch exists because of paranoia only.
                     * The WinPCAP filter is set to capture certain ICMP and UDP packets only
                     * and therefore should never end up here.
                     */
                    goto error;
                }
            }
        }

    error:
        // error indication
        newProbe.sendTime = -1;
        newProbe.recvTime = -1;

    exit:
        return newProbe;
    }
}

UdpPing::UdpPing(QObject *parent)
: Measurement(parent)
, currentStatus(Unknown)
, m_device(NULL)
, m_capture(NULL)
{
}

UdpPing::~UdpPing()
{
}

Measurement::Status UdpPing::status() const
{
    return currentStatus;
}

void UdpPing::setStatus(Status status)
{
    if (currentStatus != status)
    {
        currentStatus = status;
        emit statusChanged(status);
    }
}

bool UdpPing::prepare(NetworkManager* networkManager, const MeasurementDefinitionPtr& measurementDefinition)
{
    Q_UNUSED(networkManager);

    pcap_if_t *alldevs;
    char errbuf[PCAP_ERRBUF_SIZE] = "";
    char source[PCAP_BUF_SIZE] = "";
    char address[16] = "";
    sockaddr_any dst_addr;
    unsigned long netmask;
    struct bpf_program fcode;

    definition = measurementDefinition.dynamicCast<UdpPingDefinition>();

    memset(&dst_addr, 0, sizeof(dst_addr));

    // translate domain name to IP address
    getAddress(definition->url, &dst_addr);
    dst_addr.sin.sin_port = htons(definition->destinationPort ? definition->destinationPort : 33434);
    strncpy(address, inet_ntoa(dst_addr.sin.sin_addr), sizeof(address));

    if (pcap_createsrcstr(source, PCAP_SRC_IFLOCAL, address, NULL, NULL, errbuf) != 0)
    {
        emit error("pcap_createsrcstr: " + QString(errbuf));
        return false;
    }

    if (pcap_findalldevs_ex(source, NULL, &alldevs, errbuf) != 0)
    {
        emit error("pcap_findalldevs_ex: " + QString(errbuf));
        return false;
    }

    // take the first device
    m_device = alldevs;
    if (m_device == NULL)
    {
        emit error("pcap: no device found");
        return false;
    }

    // Open the device
    m_capture = pcap_open(m_device->name, 100, PCAP_OPENFLAG_NOCAPTURE_LOCAL, 2000, NULL, errbuf);
    if (m_capture == NULL)
    {
        emit error("pcap_open: " + QString(errbuf));
        return false;
    }

    // set filter
    if (m_device->addresses != NULL)
    {
        // Retrieve the mask of the first address of the interface
        netmask = ((struct sockaddr_in *) (m_device->addresses->netmask))->sin_addr.S_un.S_addr;
    } else
    {
        // If the interface is without an address we suppose to be in a C class network
        netmask = 0xffffff;
    }

    // capture only our UDP request and some ICMP responses
    QString filter = "(icmp and icmptype != icmp-echo) or (udp and dst host " + QString::fromLocal8Bit(address) + ")";
    if (pcap_compile(m_capture, &fcode, filter.toStdString().c_str(), 1, netmask) < 0)
    {
        pcap_freealldevs(alldevs);
        return false;
    }

    if (pcap_setfilter(m_capture, &fcode) < 0)
    {
        pcap_freealldevs(alldevs);
        return false;
    }

    // At this point, we don't need any more the device list. Free it
    pcap_freealldevs(alldevs);

    return true;
}

bool UdpPing::start()
{
    PingProbe probe;

    setStatus(UdpPing::Running);

    memset(&probe, 0, sizeof(probe));
    probe.sock = initSocket();
    if (probe.sock < 0)
    {
        emit error("initSocket");
        return false;
    }

    for (quint32 i = 0; i < definition->count; i++)
    {
        ping(&probe);
        m_pingProbes.append(probe);
    }

    closesocket(probe.sock);

    setStatus(UdpPing::Finished);
    emit finished();

    return true;
}

bool UdpPing::stop()
{
    return true;
}

ResultPtr UdpPing::result() const
{
    QVariantList res;

    foreach (const PingProbe &probe, m_pingProbes)
    {
        if (probe.sendTime > 0 && probe.recvTime > 0)
        {
            res << probe.recvTime - probe.sendTime;
        }
    }

    return ResultPtr(new Result(QDateTime::currentDateTime(), res, QVariant()));
}

int UdpPing::initSocket()
{
    int ttl = definition->ttl ? definition->ttl : 64;
    sockaddr_any src_addr;
    sockaddr_any dst_addr;
    SOCKET sock = INVALID_SOCKET;

    memset(&src_addr, 0, sizeof(src_addr));
    memset(&dst_addr, 0, sizeof(dst_addr));

    getAddress(definition->url, &dst_addr);
    dst_addr.sin.sin_port = htons(definition->destinationPort ? definition->destinationPort : 33434);

    sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock == INVALID_SOCKET)
    {
        // TODO: emit error
        return -1;
    }

    src_addr.sa.sa_family = AF_INET;
    src_addr.sin.sin_port = htons(definition->sourcePort);
    if (bind(sock, (struct sockaddr *) &src_addr, sizeof(src_addr)) != 0)
    {
        emit error("bind: " + QString(strerror(errno)));
        goto cleanup;
    }

    // set TTL
    if (setsockopt(sock, IPPROTO_IP, IP_TTL, (char *) &ttl, sizeof(ttl)) != 0)
    {
        emit error("setsockopt IP_TTL: " + QString(strerror(errno)));
        goto cleanup;
    }

    if (::connect(sock, (struct sockaddr *) &dst_addr, sizeof(dst_addr)) < 0)
    {
        emit error("connect: " + QString(strerror(errno)));
        goto cleanup;
    }

    return sock;

cleanup:
    closesocket(sock);
    return -1;
}

bool UdpPing::sendData(PingProbe *probe)
{
    if (send(probe->sock, definition->payload, sizeof(definition->payload), 0) < 0)
    {
        emit error("send: " + QString(strerror(errno)));
        return false;
    }
    return true;
}

void UdpPing::receiveData(PingProbe *probe)
{
    Q_UNUSED(probe);
}

void UdpPing::ping(PingProbe *probe)
{
    PingProbe result;
    QFuture<PingProbe> future;
    sockaddr_any dst_addr;

    // translate domain name to IP address
    getAddress(definition->url, &dst_addr);
    dst_addr.sin.sin_port = htons(definition->destinationPort ? definition->destinationPort : 33434);

    future = QtConcurrent::run(&receiveLoop, m_capture, *probe, dst_addr);

    if (sendData(probe))
    {
        future.waitForFinished();
        result = future.result();

        if (result.sendTime > 0 && result.recvTime > 0)
        {
            probe->sendTime = result.sendTime;
            probe->recvTime = result.recvTime;

            switch (result.response)
            {
            case DESTINATION_UNREACHABLE:
                emit destinationUnreachable(*probe);
                break;
            case TTL_EXCEEDED:
                emit ttlExceeded(*probe);
                break;
            case UNHANDLED_ICMP:
                emit error("Unhandled ICMP packet (type/code): " + QString::number(result.icmpType)
                           + "/" + QString::number(result.icmpCode));
                break;
            }

        } else if (result.sendTime == 0 && result.recvTime == 0)
        {
            emit error("timeout");
        } else
        {
            emit error("error receiving packets");
        }
    } else
    {
        emit error("error while sending");
    }
}

// vim: set sts=4 sw=4 et:
