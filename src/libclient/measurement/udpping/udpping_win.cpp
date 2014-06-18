#include "udpping.h"
#include "../../log/logger.h"

#include <time.h>
#include <Windows.h>
#include <mswsock.h>
#include <Mstcpip.h>
#include <pcap.h>

#include <QCryptographicHash>
#include <QThread>

LOGGER("UdpPing");

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

        if (getaddrinfo(address.toLatin1(), NULL, &hints, &result))
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

    PingProbe handleIpv4Response(const u_char *data,
                                 const pcap_pkthdr *header,
                                 const sockaddr_any &destination,
                                 int payloadSize)
    {
        char sourceAddress[INET_ADDRSTRLEN] = "";
        char destinationAddress[INET_ADDRSTRLEN] = "";
        ipAddress *source;
        quint8 *icmpType;
        quint8 *icmpCode;
        quint8 *ipProto;
        PingProbe probe = {0};
        QByteArray payload;

        if (data == NULL || header == NULL)
        {
            return probe;
        }

        ipProto = const_cast<quint8 *>(data + 23);
        source = reinterpret_cast<ipAddress *>(const_cast<u_char *>(data + 26));
        icmpType = const_cast<quint8 *>(data + 34);
        icmpCode = const_cast<quint8 *>(data + 35);

        // source address of the response packet
        RtlIpv4AddressToStringA(&(reinterpret_cast<inAddress *>(source))->v4,
                                sourceAddress);
        // destination of request packet
        RtlIpv4AddressToStringA(&destination.sin.sin_addr, destinationAddress);

        switch (*ipProto)
        {
        case 17:
            // UDP packet
            probe.type = udpping::UDP;

            if (!strncmp(sourceAddress, destinationAddress, INET_ADDRSTRLEN))
            {
                /*
                 * UDP response
                 *
                 * It is quite possible that the UDP response is not caused by our request.
                 * This however cannot be determined, as such a response most likely will not
                 * echo back our packet/payload which we could analyse.
                 */
                probe.recvTime = header->ts.tv_sec * 1e6 +
                                 header->ts.tv_usec;
                probe.response = udpping::UDP_RESPONSE;
                getAddress(sourceAddress, &probe.source);
            }
            else
            {
                // UDP request
                probe.sendTime = header->ts.tv_sec * 1e6 + header->ts.tv_usec;
                payload = QByteArray::fromRawData(reinterpret_cast<char *>(const_cast<u_char *>(data + 42)),
                                                  payloadSize);
                probe.hash = QCryptographicHash::hash(payload, QCryptographicHash::Sha256).toHex();
            }

            break;

        case 1:
            // ICMP response
            probe.type = udpping::ICMP;
            getAddress(sourceAddress, &probe.source);

            if ((*icmpType == 3 && *icmpCode == 3) ||
                (*icmpType == 11 && *icmpCode == 0))
            {
                payload = QByteArray::fromRawData(reinterpret_cast<char *>(const_cast<u_char *>(data + 70)),
                                                  payloadSize);
                probe.hash = QCryptographicHash::hash(payload, QCryptographicHash::Sha256).toHex();
                /*
                 * 'source' points to the destination field of the original
                 * packet, which is sent as the ICMP payload.
                 */
                source = reinterpret_cast<ipAddress *>(const_cast<u_char *>(data + 58));
                RtlIpv4AddressToStringA(&(reinterpret_cast<inAddress *>(source))->v4,
                                        sourceAddress);

                if (strncmp(sourceAddress, destinationAddress,
                            INET_ADDRSTRLEN) == 0)
                {
                    probe.recvTime = header->ts.tv_sec * 1e6 +
                                     header->ts.tv_usec;
                    probe.response = *icmpType == 3
                                     ? udpping::DESTINATION_UNREACHABLE
                                     : udpping::TTL_EXCEEDED;
                }
            }
            else
            {
                /*
                 * An unhandled ICMP packet has been captured. We need to check
                 * this and handle it if needed.
                 */
                probe.icmpType = *icmpType;
                probe.icmpCode = *icmpCode;
                probe.response = udpping::UNHANDLED_ICMP;
            }

            break;

        default:
            /*
             * This else-branch exists because of paranoia only. The WinPCAP
             * filter is set to capture certain ICMP and UDP packets only and
             * therefore should never end up here.
             */
            probe.icmpType = udpping::UNKNOWN;
            probe.sendTime = -1;
            probe.recvTime = -1;
        }

        return probe;
    }

    PingProbe handleIpv6Response(const u_char *data,
                                 const pcap_pkthdr *header,
                                 const sockaddr_any &destination,
                                 quint32 payloadSize)
    {
        char sourceAddress[INET6_ADDRSTRLEN] = "";
        char destinationAddress[INET6_ADDRSTRLEN] = "";
        ipAddress *source;
        quint8 *icmpType;
        quint8 *icmpCode;
        quint8 *ipProto;
        PingProbe probe = {0};
        QByteArray payload;

        if (data == NULL || header == NULL)
        {
            return probe;
        }

        ipProto = const_cast<quint8 *>(data + 20);
        source = reinterpret_cast<ipAddress *>(const_cast<u_char *>(data + 22));
        icmpType = const_cast<quint8 *>(data + 54);
        icmpCode = const_cast<quint8 *>(data + 55);

        RtlIpv6AddressToStringA(&(reinterpret_cast<inAddress *>(source))->v6,
                                sourceAddress);
        RtlIpv6AddressToStringA(&destination.sin6.sin6_addr,
                                destinationAddress);

        switch (*ipProto)
        {
        case 17:
            // UDP packet
            probe.type = udpping::UDP;

            if (!strncmp(sourceAddress, destinationAddress, INET6_ADDRSTRLEN))
            {
                // UDP response
                probe.recvTime = header->ts.tv_sec * 1e6 +
                                 header->ts.tv_usec;
                probe.response = udpping::UDP_RESPONSE;
                getAddress(sourceAddress, &probe.source);
            }
            else
            {
                // UDP request
                probe.sendTime = header->ts.tv_sec * 1e6 + header->ts.tv_usec;
                payload = QByteArray::fromRawData(reinterpret_cast<char *>(const_cast<u_char *>(data + 62)),
                                                  payloadSize);
                probe.hash = QCryptographicHash::hash(payload, QCryptographicHash::Sha256).toHex();
            }

            break;

        case 58:
            // ICMPv6 response
            probe.type = udpping::ICMP;
            getAddress(sourceAddress, &probe.source);

            if ((*icmpType == 1 && *icmpCode == 4) ||
                (*icmpType == 3 && *icmpCode == 0))
            {
                payload = QByteArray::fromRawData(reinterpret_cast<char *>(const_cast<u_char *>(data + 110)),
                                                  payloadSize);
                probe.hash = QCryptographicHash::hash(payload, QCryptographicHash::Sha256).toHex();

                source = reinterpret_cast<ipAddress *>(const_cast<u_char *>(data + 86));
                RtlIpv6AddressToStringA(&(reinterpret_cast<inAddress *>(source))->v6,
                                        sourceAddress);

                if (strncmp(sourceAddress, destinationAddress,
                            INET6_ADDRSTRLEN) == 0)
                {
                    probe.recvTime = header->ts.tv_sec * 1e6 +
                                     header->ts.tv_usec;
                    probe.response = *icmpType == 1
                                     ? udpping::DESTINATION_UNREACHABLE
                                     : udpping::TTL_EXCEEDED;
                }
            }
            else
            {
                /*
                 * An unhandled ICMP packet has been captured. We need to check
                 * this and handle it if needed.
                 */
                probe.icmpType = *icmpType;
                probe.icmpCode = *icmpCode;
                probe.response = udpping::UNHANDLED_ICMP;
            }

            break;

        default:
            /*
             * This else-branch exists because of paranoia only. The WinPCAP
             * filter is set to capture certain ICMP and UDP packets only and
             * therefore should never end up here.
             */
            probe.icmpType = udpping::UNKNOWN;
            probe.sendTime = -1;
            probe.recvTime = -1;
        }

        return probe;
    }

    QVector<PingProbe> receiveLoop(pcap_t *capture,
                                   sockaddr_any destination,
                                   quint32 maxPackets,
                                   quint32 payloadSize)
    {
        int res;
        int timeouts = 0;
        const u_char *data;
        quint16 *ipVersion;
        pcap_pkthdr *header;
        QVector<PingProbe> probes;
        PingProbe probe = {0};

        while ((quint32) probes.size() < maxPackets)
        {
            res = pcap_next_ex(capture, &header, &data);

            switch (res)
            {
            case 0:

                // timed out
                if (timeouts++ == 10)
                {
                    LOG_WARNING("too many timeouts");
                    return probes;
                }

                break;

            case -1:
                // error
                LOG_WARNING("pcap_next_ex returned error");
                return probes;

            default:
                // packet received
                ipVersion = reinterpret_cast<quint16 *>(const_cast<u_char *>(data + 12));

                switch (*ipVersion)
                {
                case 0x8:
                    probe = handleIpv4Response(data, header,
                                               destination,
                                               payloadSize);

                    foreach (const PingProbe &p, probes)
                    {
                        if (p == probe)
                        {
                            // don't add duplicate probe
                            continue;
                        }
                    }

                    probes.append(probe);
                    break;

                case 0xdd86:
                    probe = handleIpv6Response(data, header,
                                               destination,
                                               payloadSize);

                    foreach (const PingProbe &p, probes)
                    {
                        if (p == probe)
                        {
                            // don't add duplicate probe
                            continue;
                        }
                    }

                    probes.append(probe);
                    break;
                }
            }
        }

        return probes;
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
, m_pingProbes(0)
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

bool UdpPing::prepare(NetworkManager *networkManager, const MeasurementDefinitionPtr &measurementDefinition)
{
    Q_UNUSED(networkManager);

    pcap_if_t *alldevs;
    char errbuf[PCAP_ERRBUF_SIZE] = "";
    char source[PCAP_BUF_SIZE] = "";
    char address[INET6_ADDRSTRLEN] = "";
    struct bpf_program fcode;

    definition = measurementDefinition.dynamicCast<UdpPingDefinition>();

    if (definition->payload > 1400)
    {
        emit error("payload is too large (> 1400 bytes)");
        return false;
    }

    if (definition->receiveTimeout > 60000)
    {
        emit error("receive timeout is too large (> 60 s)");
        return false;
    }

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
    }
    else if (m_destAddress.sa.sa_family == AF_INET6)
    {
        RtlIpv6AddressToStringA(&m_destAddress.sin6.sin6_addr, address);
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

    // capture only our UDP request and some ICMP/UDP responses
    QString filter = QString("(((icmp or icmp6) and icmptype != icmp-echo and dst host %1)"
                             " or (udp and dst port %2))").arg(
                     address).arg(definition->destinationPort);

    if (pcap_compile(m_capture, &fcode, filter.toLatin1(), 1, 0) < 0)
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
    probe.sock = initSocket();

    if (probe.sock < 0)
    {
        emit error("initSocket");
        delete[] m_payload;
        return false;
    }

    ping(&probe);
    closesocket(probe.sock);
    delete[] m_payload;

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

    return ResultPtr(new Result(startDateTime(), QDateTime::currentDateTime(),
                                res, QVariant()));
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
    QVector<PingProbe> result;
    QFuture<QVector<PingProbe>> future;
    PingProbe newProbe = {0};

    future = QtConcurrent::run(&receiveLoop, m_capture, m_destAddress,
                               definition->count * 2, definition->payload);

    for (quint32 i = 0; i < definition->count; i++)
    {
        if (!sendData(probe))
        {
            emit error("error while sending");
        }

        QThread::usleep(definition->interval * 1000);
    }

    future.waitForFinished();
    result = future.result();

    if (static_cast<quint32>(result.size()) < definition->count * 2)
    {
        emit error("error while capturing packets");
        return;
    }

    /*
     * Should there be ICMP responses, they need to be processed first as they can
     * be matched against UDP requests by comparing their payload hash. Processed
     * packets are marked and are then not used for any future matching.
     *
     * Possible remaining UDP responses are processed afterwards. Since UDP
     * requests and responses cannot be matched by their payload, they are matched
     * according to their capturing order, i.e.
     * 1st request -> 1st response, 2nd request -> 2nd response, and so on.
     */
    for (QVector<PingProbe>::iterator p = result.begin(); p != result.end(); p++)
    {
        // match a request packet ...
        if (p->marked || p->type != udpping::UDP || p->response == udpping::UDP_RESPONSE)
        {
            continue;
        }

        for (QVector<PingProbe>::iterator q = result.begin(); q != result.end(); q++)
        {
            // ... with a response packet
            if (q->marked || (q->type != udpping::ICMP || p->hash != q->hash))
            {
                continue;
            }

            p->marked = true;
            q->marked = true;

            newProbe.sendTime = p->sendTime;
            newProbe.recvTime = q->recvTime;
            newProbe.source = q->source;
            newProbe.response = q->response;
            newProbe.icmpType = q->icmpType;
            newProbe.icmpCode = q->icmpCode;

            m_pingProbes.append(newProbe);

            switch (newProbe.response)
            {
            case udpping::DESTINATION_UNREACHABLE:
                emit destinationUnreachable(newProbe);
                break;

            case udpping::TTL_EXCEEDED:
                emit ttlExceeded(newProbe);
                break;

            case udpping::UNHANDLED_ICMP:
                emit error("Unhandled ICMP packet (type/code): " +
                           QString::number(newProbe.icmpType) + "/" +
                           QString::number(newProbe.icmpCode));
                break;
            }

            break;
        }
    }

    // process possible UDP responses
    for (QVector<PingProbe>::iterator p = result.begin(); p != result.end(); p++)
    {
        if (p->marked || p->type != udpping::UDP || p->response == udpping::UDP_RESPONSE)
        {
            continue;
        }

        for (QVector<PingProbe>::iterator q = result.begin(); q != result.end(); q++)
        {
            if (!q->marked && (q->type == udpping::UDP && q->response == udpping::UDP_RESPONSE))
            {
                p->marked = true;
                q->marked = true;

                newProbe.sendTime = p->sendTime;
                newProbe.recvTime = q->recvTime;
                newProbe.source = q->source;
                newProbe.response = q->response;
                newProbe.icmpType = q->icmpType;
                newProbe.icmpCode = q->icmpCode;

                m_pingProbes.append(newProbe);

                emit udpResponse(newProbe);
                break;
            }
        }
    }
}

// vim: set sts=4 sw=4 et:
