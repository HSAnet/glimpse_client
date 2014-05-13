#include "udpping.h"

#include <time.h>
#include <Windows.h>
#include <mswsock.h>
#include <Mstcpip.h>
#include <pcap.h>

namespace
{
    union ipAddress
    {
        quint32 v4;
        quint8 v6[16];
    };

    union inAddress
    {
        struct in_addr v4;
        struct in6_addr v6;
    };

    bool getAddress(const QString &address, sockaddr_any *addr)
    {
        struct addrinfo hints;
        struct addrinfo *rp = NULL, *result = NULL;

        memset(&hints, 0, sizeof(hints));
        hints.ai_family = AF_UNSPEC;

        if (getaddrinfo(address.toStdString().c_str(), NULL, &hints, &result))
        {
            return false;
        }

        for (rp = result; rp && rp->ai_family != AF_INET &&
             rp->ai_family != AF_INET6; rp = rp->ai_next)
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

    bool handleIpv4Response(const u_char *data,
                            const pcap_pkthdr *header,
                            const sockaddr_any &destination,
                            PingProbe *newProbe)
    {
        char sourceAddress[INET_ADDRSTRLEN] = "";
        char destinationAddress[INET_ADDRSTRLEN] = "";
        ipAddress *source;
        quint8 *icmpType;
        quint8 *icmpCode;
        quint8 *ipProto;

        if (data == NULL || header == NULL || newProbe == NULL)
        {
            goto exit;
        }

        ipProto = (quint8 *)(data + 23);
        source = (ipAddress *)(data + 26);
        icmpType = (quint8 *)(data + 34);
        icmpCode = (quint8 *)(data + 35);

        // source address of the response packet
        RtlIpv4AddressToStringA(&((inAddress *)source)->v4, sourceAddress);
        sourceAddress[INET_ADDRSTRLEN - 1] = '\0';
        // destination of request packet
        RtlIpv4AddressToStringA(&destination.sin.sin_addr, destinationAddress);
        destinationAddress[INET_ADDRSTRLEN - 1] = '\0';

        if (*ipProto == 17)
        {
            // UDP request
            newProbe->sendTime = header->ts.tv_sec * 1e6 + header->ts.tv_usec;

            // tell the caller to continue capturing packets
            return false;
        }
        else if (*ipProto == 1)
        {
            // ICMP response

            getAddress(sourceAddress, &newProbe->source);

            if ((*icmpType == 3 && *icmpCode == 3) ||
                    (*icmpType == 11 && *icmpCode == 0))
            {
                /*
                 * 'source' points to the destination field of the original
                 * packet, which is sent as the ICMP payload.
                 */
                source = (ipAddress *) (data + 58);
                RtlIpv4AddressToStringA(&((inAddress *)source)->v4,
                                        sourceAddress);
                sourceAddress[INET_ADDRSTRLEN - 1] = '\0';

                if (strncmp(sourceAddress, destinationAddress,
                            INET_ADDRSTRLEN) == 0)
                {
                    newProbe->recvTime = header->ts.tv_sec * 1e6 +
                            header->ts.tv_usec;
                    newProbe->response = *icmpType == 3
                            ? udpping::DESTINATION_UNREACHABLE
                            : udpping::TTL_EXCEEDED;
                    goto exit;
                }
            }
            else
            {
                /*
                 * An unhandled ICMP packet has been captured. We need to check
                 * this and handle it if needed.
                 */
                newProbe->icmpType = *icmpType;
                newProbe->icmpCode = *icmpCode;
                newProbe->response = udpping::UNHANDLED_ICMP;
                goto exit;
            }
        }
        else
        {
            /*
             * This else-branch exists because of paranoia only. The WinPCAP
             * filter is set to capture certain ICMP and UDP packets only and
             * therefore should never end up here.
             */
            goto error;
        }

    error:
        // error indication
        newProbe->sendTime = -1;
        newProbe->recvTime = -1;

    exit:
        return true;
    }

    bool handleIpv6Response(const u_char *data,
                            const pcap_pkthdr *header,
                            const sockaddr_any &destination,
                            PingProbe *newProbe)
    {
        char sourceAddress[INET6_ADDRSTRLEN] = "";
        char destinationAddress[INET6_ADDRSTRLEN] = "";
        ipAddress *source;
        quint8 *icmpType;
        quint8 *icmpCode;
        quint8 *ipProto;

        if (data == NULL || header == NULL || newProbe == NULL)
        {
            goto exit;
        }

        ipProto = (quint8 *)(data + 20);
        source = (ipAddress *)(data + 22);
        icmpType = (quint8 *)(data + 54);
        icmpCode = (quint8 *)(data + 55);

        RtlIpv6AddressToStringA(&((inAddress *)source)->v6, sourceAddress);
        sourceAddress[INET6_ADDRSTRLEN - 1] = '\0';
        RtlIpv6AddressToStringA(&destination.sin6.sin6_addr,
                                destinationAddress);
        destinationAddress[INET6_ADDRSTRLEN - 1] = '\0';

        if (*ipProto == 17)
        {
            // UDP request
            newProbe->sendTime = header->ts.tv_sec * 1e6 + header->ts.tv_usec;

            return false;
        }
        else if (*ipProto == 58)
        {
            // ICMPv6 response

            getAddress(sourceAddress, &newProbe->source);

            if ((*icmpType == 1 && *icmpCode == 4) ||
                    (*icmpType == 3 && *icmpCode == 0))
            {
                source = (ipAddress *) (data + 86);
                RtlIpv6AddressToStringA(&((inAddress *)source)->v6,
                                        sourceAddress);
                sourceAddress[INET6_ADDRSTRLEN - 1] = '\0';

                if (strncmp(sourceAddress, destinationAddress,
                            INET6_ADDRSTRLEN) == 0)
                {
                    newProbe->recvTime = header->ts.tv_sec * 1e6 +
                            header->ts.tv_usec;
                    newProbe->response = *icmpType == 1
                            ? udpping::DESTINATION_UNREACHABLE
                            : udpping::TTL_EXCEEDED;
                    goto exit;
                }
            }
            else
            {
                /*
                 * An unhandled ICMP packet has been captured. We need to check
                 * this and handle it if needed.
                 */
                newProbe->icmpType = *icmpType;
                newProbe->icmpCode = *icmpCode;
                newProbe->response = udpping::UNHANDLED_ICMP;
                goto exit;
            }
        }
        else
        {
            /*
             * This else-branch exists because of paranoia only. The WinPCAP
             * filter is set to capture certain ICMP and UDP packets only and
             * therefore should never end up here.
             */
            goto error;
        }

    error:
        // error indication
        newProbe->sendTime = -1;
        newProbe->recvTime = -1;

    exit:
        // tell the caller to stop capturing packets
        return true;
    }

    PingProbe receiveLoop(pcap_t *capture, PingProbe probe, sockaddr_any destination)
    {
        int res;
        const u_char *data;
        pcap_pkthdr *header;
        quint16 *ipVersion;
        PingProbe newProbe;

        memcpy(&newProbe, &probe, sizeof(newProbe));

        for (;;)
        {
            res = pcap_next_ex(capture, &header, &data);

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

                ipVersion = (quint16 *) (data + 12);

                if (*ipVersion == 0x8)
                {
                    if (handleIpv4Response(data, header, destination,
                                           &newProbe))
                    {
                        goto exit;
                    }
                }
                else if (*ipVersion = 0xdd86)
                {
                    if (handleIpv6Response(data, header, destination, &newProbe))
                    {
                        goto exit;
                    }
                }
                else
                {
                    // unreachable
                    goto exit;
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

    void randomizePayload(char *payload, const quint32 size)
    {
        char chars[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890";

        for (quint32 i = 0; i < size; i++)
        {
            payload[i] = chars[qrand() % strlen(chars)];
        }

        if (size > 15)
        {
            // ignore terminating null character
            strncpy(&payload[size - 15], " measure-it.net", 15);
        }
    }
}

UdpPing::UdpPing(QObject *parent)
: Measurement(parent)
, currentStatus(Unknown)
, m_device(NULL)
, m_capture(NULL)
, m_destAddress()
, m_payload(NULL)
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
    char address[INET6_ADDRSTRLEN] = "";
    struct bpf_program fcode;

    definition = measurementDefinition.dynamicCast<UdpPingDefinition>();

    memset(&m_destAddress, 0, sizeof(m_destAddress));

    // resolve
    if (!getAddress(definition->url, &m_destAddress))
    {
        return false;
    }

    m_destAddress.sin.sin_port = htons(definition->destinationPort ? definition->destinationPort : 33434);
    if (m_destAddress.sa.sa_family == AF_INET)
    {
        RtlIpv4AddressToStringA(&m_destAddress.sin.sin_addr, address);
        address[INET_ADDRSTRLEN - 1] = '\0';
    }
    else if (m_destAddress.sa.sa_family == AF_INET6)
    {
        RtlIpv6AddressToStringA(&m_destAddress.sin6.sin6_addr, address);
        address[INET6_ADDRSTRLEN - 1] = '\0';
    }

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
    m_capture = pcap_open(m_device->name, 110 + definition->payload,
                          PCAP_OPENFLAG_NOCAPTURE_LOCAL, 2000, NULL,
                          errbuf);

    if (m_capture == NULL)
    {
        emit error("pcap_open: " + QString(errbuf));
        return false;
    }

    // capture only our UDP request and some ICMP responses
    QString filter = "((icmp or icmp6) and icmptype != icmp-echo) or (udp and dst host " + QString::fromLocal8Bit(address) + ")";
    if (pcap_compile(m_capture, &fcode, filter.toStdString().c_str(), 1, 0) < 0)
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

    // include null-character
    m_payload = new char[definition->payload + 1];
    if (m_payload == NULL)
    {
        return false;
    }
    memset(m_payload, 0, definition->payload + 1);
    setStartDateTime(QDateTime::currentDateTime());

    setStatus(UdpPing::Running);

    memset(&probe, 0, sizeof(probe));

    probe.sock = initSocket();
    if (probe.sock < 0)
    {
        emit error("initSocket");
        free(m_payload);
        return false;
    }

    for (quint32 i = 0; i < definition->count; i++)
    {
        ping(&probe);
        m_pingProbes.append(probe);
    }

    closesocket(probe.sock);

    setStatus(UdpPing::Finished);
    free(m_payload);
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

    return ResultPtr(new Result(startDateTime(), QDateTime::currentDateTime(), res, QVariant()));
}

int UdpPing::initSocket()
{
    int ttl = definition->ttl ? definition->ttl : 64;
    sockaddr_any src_addr;
    SOCKET sock = INVALID_SOCKET;

    memset(&src_addr, 0, sizeof(src_addr));

    sock = socket(m_destAddress.sa.sa_family, SOCK_DGRAM, IPPROTO_UDP);
    if (sock == INVALID_SOCKET)
    {
        // TODO: emit error
        return -1;
    }

    src_addr.sa.sa_family = m_destAddress.sa.sa_family;
    if (m_destAddress.sa.sa_family == AF_INET)
    {
        src_addr.sin.sin_port = htons(definition->sourcePort);
    }
    else if (m_destAddress.sa.sa_family == AF_INET6)
    {
        src_addr.sin6.sin6_port = htons(definition->sourcePort);
    }

    if (bind(sock, (struct sockaddr *) &src_addr, sizeof(src_addr)) != 0)
    {
        emit error("bind: " + QString(strerror(errno)));
        goto cleanup;
    }

    // set TTL
    if (m_destAddress.sa.sa_family == AF_INET)
    {
        if (setsockopt(sock, IPPROTO_IP, IP_TTL, (char *) &ttl,
                       sizeof(ttl)) != 0)
        {
            emit error("setsockopt IP_TTL: " + QString(strerror(errno)));
            goto cleanup;
        }
    }
    else if (m_destAddress.sa.sa_family == AF_INET6)
    {
        if (setsockopt(sock, IPPROTO_IPV6, IPV6_UNICAST_HOPS, (char *) &ttl,
                       sizeof(ttl)) != 0)
        {
            emit error("setsockopt IPV6_UNICAST_HOPS: " +
                       QString(strerror(errno)));
            goto cleanup;
        }
    }

    return sock;

cleanup:
    closesocket(sock);
    return -1;
}

bool UdpPing::sendData(PingProbe *probe)
{
    // randomize payload to prevent caching
    randomizePayload(m_payload, definition->payload);

    if (sendto(probe->sock, m_payload, definition->payload, 0,
               (sockaddr *)&m_destAddress, sizeof(m_destAddress)) < 0)
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

    future = QtConcurrent::run(&receiveLoop, m_capture, *probe, m_destAddress);

    if (sendData(probe))
    {
        future.waitForFinished();
        result = future.result();

        if (result.sendTime > 0 && result.recvTime > 0)
        {
            probe->sendTime = result.sendTime;
            probe->recvTime = result.recvTime;
            probe->source = result.source;

            switch (result.response)
            {
            case udpping::DESTINATION_UNREACHABLE:
                emit destinationUnreachable(*probe);
                break;
            case udpping::TTL_EXCEEDED:
                emit ttlExceeded(*probe);
                break;
            case udpping::UNHANDLED_ICMP:
                emit error("Unhandled ICMP packet (type/code): " + QString::number(result.icmpType)
                           + "/" + QString::number(result.icmpCode));
                break;
            }

        } else if (result.sendTime == 0 && result.recvTime == 0)
        {
            emit timeout(*probe);
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
