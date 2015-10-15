#include "ping.h"
#include "../../log/logger.h"
#include "../../client.h"
#include "../../trafficbudgetmanager.h"

#include <time.h>
#include <Windows.h>
#include <mswsock.h>
#include <Mstcpip.h>
#include <pcap.h>
#include <numeric>

#include <QCryptographicHash>
#include <QThread>
#include <QtMath>

LOGGER("Ping");

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
        quint16 *tcpFlags;
        PingProbe probe;
        QByteArray payload;

        if (data == NULL || header == NULL)
        {
            return probe;
        }

        ipProto = const_cast<quint8 *>(data + 23);
        source = reinterpret_cast<ipAddress *>(const_cast<u_char *>(data + 26));
        icmpType = const_cast<quint8 *>(data + 34);
        icmpCode = const_cast<quint8 *>(data + 35);
        tcpFlags = reinterpret_cast<quint16 *>(const_cast<u_char *>(data + 46));

        // source address of the response packet
        RtlIpv4AddressToStringA(&(reinterpret_cast<inAddress *>(source))->v4,
                                sourceAddress);
        // destination of request packet
        RtlIpv4AddressToStringA(&destination.sin.sin_addr, destinationAddress);

        switch (*ipProto)
        {
        case 17:
            // UDP packet
            probe.type = ping::UDP;

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
                probe.response = ping::UDP_RESPONSE;
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

        case 6:
            // TCP
            probe.type = ping::TCP;

            if (!strncmp(sourceAddress, destinationAddress, INET_ADDRSTRLEN))
            {
                // TCP response (SYN-ACK or RST)
                probe.recvTime = header->ts.tv_sec * 1e6 +
                                 header->ts.tv_usec;
                getAddress(sourceAddress, &probe.source);

                // swap bytes of tcp flags
                switch ((((*tcpFlags << 8) & 0xff00) + ((*tcpFlags >> 8) & 0xff)) & 0x16)
                {
                // SYN and ACK flags set
                case 0x12:
                    probe.response = ping::TCP_CONNECT;
                    break;

                // RST and ACK flags set
                case 0x14:
                    probe.response = ping::TCP_RESET;
                    break;

                default:
                    break;
                }
            }
            else
            {
                // TCP request
                probe.sendTime = header->ts.tv_sec * 1e6 + header->ts.tv_usec;
            }

            break;

        case 1:
            // ICMP response
            probe.type = ping::ICMP;
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
                                     ? ping::DESTINATION_UNREACHABLE
                                     : ping::TTL_EXCEEDED;
                }
                else
                {
                    // This is not our packet and can be dismissed.
                    probe.response = ping::INVALID_ICMP;
                    break;
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
                probe.response = ping::UNHANDLED_ICMP;
            }

            break;

        default:
            /*
             * This else-branch exists because of paranoia only. The WinPCAP
             * filter is set to capture certain ICMP and UDP packets only and
             * therefore should never end up here.
             */
            probe.icmpType = ping::UNKNOWN;
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
        quint16 *tcpFlags;
        PingProbe probe;
        QByteArray payload;

        if (data == NULL || header == NULL)
        {
            return probe;
        }

        ipProto = const_cast<quint8 *>(data + 20);
        source = reinterpret_cast<ipAddress *>(const_cast<u_char *>(data + 22));
        icmpType = const_cast<quint8 *>(data + 54);
        icmpCode = const_cast<quint8 *>(data + 55);
        tcpFlags = reinterpret_cast<quint16 *>(const_cast<u_char *>(data + 66));

        RtlIpv6AddressToStringA(&(reinterpret_cast<inAddress *>(source))->v6,
                                sourceAddress);
        RtlIpv6AddressToStringA(&destination.sin6.sin6_addr,
                                destinationAddress);

        switch (*ipProto)
        {
        case 17:
            // UDP packet
            probe.type = ping::UDP;

            if (!strncmp(sourceAddress, destinationAddress, INET6_ADDRSTRLEN))
            {
                // UDP response
                probe.recvTime = header->ts.tv_sec * 1e6 +
                                 header->ts.tv_usec;
                probe.response = ping::UDP_RESPONSE;
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

        case 6:
            // TCP
            probe.type = ping::TCP;

            if (!strncmp(sourceAddress, destinationAddress, INET_ADDRSTRLEN))
            {
                // TCP response (SYN-ACK or RST)
                probe.recvTime = header->ts.tv_sec * 1e6 +
                                 header->ts.tv_usec;
                getAddress(sourceAddress, &probe.source);

                // swap bytes of tcp flags
                switch ((((*tcpFlags << 8) & 0xff00) + ((*tcpFlags >> 8) & 0xff)) & 0x16)
                {
                // SYN and ACK flags set
                case 0x12:
                    probe.response = ping::TCP_CONNECT;
                    break;

                // RST and ACK flags set
                case 0x14:
                    probe.response = ping::TCP_RESET;
                    break;

                default:
                    break;
                }
            }
            else
            {
                // TCP request
                probe.sendTime = header->ts.tv_sec * 1e6 + header->ts.tv_usec;
            }

            break;

        case 58:
            // ICMPv6 response
            probe.type = ping::ICMP;
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
                                     ? ping::DESTINATION_UNREACHABLE
                                     : ping::TTL_EXCEEDED;
                }
                else
                {
                    // This is not our packet and can be dismissed.
                    probe.response = ping::INVALID_ICMP;
                    break;
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
                probe.response = ping::UNHANDLED_ICMP;
            }

            break;

        default:
            /*
             * This else-branch exists because of paranoia only. The WinPCAP
             * filter is set to capture certain ICMP and UDP packets only and
             * therefore should never end up here.
             */
            probe.icmpType = ping::UNKNOWN;
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
        PingProbe probe;

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

                    if (probe.response == ping::INVALID_ICMP)
                    {
                        // invalid packet
                        continue;
                    }

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

                    if (probe.response == ping::INVALID_ICMP)
                    {
                        // invalid packet
                        continue;
                    }

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

    QByteArray randomizePayload(const quint32 size)
    {
        char chars[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890";
        QByteArray payload;

        payload.resize(size);

        for (quint32 i = 0; i < (size > 15 ? size - 15 : size); i++)
        {
            payload[i] = chars[qrand() % strlen(chars)];
        }

        if (size > 15)
        {
            // ignore terminating null character
            payload.insert(size - 15, " measure-it.net", 15);
        }

        return payload;
    }
}

Ping::Ping(QObject *parent)
: Measurement(parent)
, currentStatus(Unknown)
, m_pingProbes(0)
, m_pingsSent(0)
, m_pingsReceived(0)
, m_device(NULL)
, m_capture(NULL)
, m_destAddress()
, stream(&process)
{
    connect(this, SIGNAL(error(const QString &)), this,
            SLOT(setErrorString(const QString &)));
}

Ping::~Ping()
{
    if (!definition.isNull() && definition->type == ping::System)
    {
        process.kill();
        process.waitForFinished(500);
    }
}

Measurement::Status Ping::status() const
{
    return currentStatus;
}

void Ping::setStatus(Status status)
{
    if (currentStatus != status)
    {
        currentStatus = status;
        emit statusChanged(status);
    }
}

bool Ping::prepare(NetworkManager *networkManager, const MeasurementDefinitionPtr &measurementDefinition)
{
    Q_UNUSED(networkManager);

    pcap_if_t *alldevs;
    char errbuf[PCAP_ERRBUF_SIZE] = "";
    char source[PCAP_BUF_SIZE] = "";
    char address[INET6_ADDRSTRLEN] = "";
    struct bpf_program fcode;

    definition = measurementDefinition.dynamicCast<PingDefinition>();

    if (definition.isNull())
    {
        setErrorString("Definition is empty");
        return false;
    }

    if (definition->type != ping::System)
    {
        if (definition->payload > 1400)
        {
            setErrorString("payload is too large (> 1400 bytes)");
            return false;
        }
    }

    if (definition->receiveTimeout > 60000)
    {
        setErrorString("receive timeout is too large (> 60 s)");
        return false;
    }

    if (!Client::instance()->trafficBudgetManager()->addUsedTraffic(estimateTraffic()))
    {
        setErrorString("not enough traffic available");
        return false;
    }

    if (definition->type == ping::System)
    {
        connect(&process, SIGNAL(started()), this, SLOT(started()));
        connect(&process, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(finished(int, QProcess::ExitStatus)));
        connect(&process, SIGNAL(readyRead()), this, SLOT(readyRead()));
        // We exit here since there's no need to proceed further.
        return true;
    }

    memset(&m_destAddress, 0, sizeof(m_destAddress));

    // resolve
    if (!getAddress(definition->host, &m_destAddress))
    {
        setErrorString(QString("could not resolve hostname '%1'").arg(definition->host));
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
    else
    {
        setErrorString(QString("unknown address family '%1'").arg(
                           m_destAddress.sa.sa_family));
        return false;
    }

    m_destIp = address;

    if (pcap_createsrcstr(source, PCAP_SRC_IFLOCAL, address, NULL, NULL, errbuf) != 0)
    {
        setErrorString("pcap_createsrcstr: " + QString(errbuf));
        return false;
    }

    if (pcap_findalldevs_ex(source, NULL, &alldevs, errbuf) != 0)
    {
        setErrorString("pcap_findalldevs_ex: " + QString(errbuf));
        return false;
    }

    // take the first device
    m_device = alldevs;

    if (m_device == NULL)
    {
        setErrorString("pcap: no device found");
        return false;
    }

    // Open the device
    m_capture = pcap_open(m_device->name, 110 + definition->payload,
                          PCAP_OPENFLAG_NOCAPTURE_LOCAL, 2000, NULL,
                          errbuf);

    if (m_capture == NULL)
    {
        setErrorString("pcap_open: " + QString(errbuf));
        return false;
    }

    QString filter;

    if (definition->type == ping::Udp)
    {
        // capture only our UDP request and some ICMP/UDP responses
        filter = QString("(((icmp or icmp6) and icmptype != icmp-echo)"
                         " or (udp and dst port %2 and dst host %1))").arg(
                     address).arg(definition->destinationPort);
    }
    else if (definition->type == ping::Tcp)
    {
        // 0x02: SYN
        // 0x12: SYN-ACK
        // 0x14: RST-ACK
        filter = QString("(tcp and dst port %1 and src port %2 and "
                         "((dst host %3 and "
                         "tcp[tcpflags] & 0x16 == 0x2) or (src host %3 and ("
                         "tcp[tcpflags] & 0x16 == 0x12 or "
                         "tcp[tcpflags] & 0x16 == 0x14)))) or ("
                         "(icmp or icmp6) and icmptype != icmp-echo)").arg(
                     definition->destinationPort).arg(definition->sourcePort).arg(address);
    }
    else
    {
        setErrorString(QString("Unknown ping type '%1'").arg(definition->type));
        return false;
    }

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

bool Ping::start()
{
    PingProbe probe;

    setStatus(Ping::Running);

    if (definition->type == ping::System)
    {
        QStringList args;

        args << "-n" << QString::number(definition->count)
             << "-4" // ipv4
             << "-w" << QString::number(definition->receiveTimeout)
             << definition->host;

        process.kill();
        process.start("ping", args);

        return true;
    }

    probe.sock = initSocket();

    if (probe.sock < 0)
    {
        setErrorString("Failed to initialize socket");
        return false;
    }

    ping(&probe);
    closesocket(probe.sock);

    setStatus(Ping::Finished);
    emit Measurement::finished();

    return true;
}

bool Ping::stop()
{
    if (definition->type == ping::System)
    {
        process.kill();
    }

    return true;
}

Result Ping::result() const
{
    QVariantList res;
    QVariantList roundTripMs;
    float avg = 0.0;
    QList<float> tmpPingTime = pingTime;

    // calculate average and fill ping times
    foreach (float val, tmpPingTime)
    {
        roundTripMs << val;

        // ignore timeouts, i.e. ping times with 0
        if (fabs(val - 0.0) < std::numeric_limits<float>::epsilon())
        {
            continue;
        }

        avg += val;
    }

    // exclude timeouts from the stats
    tmpPingTime.removeAll(0.0);

    // get max and min
    QList<float>::const_iterator it = std::max_element(tmpPingTime.begin(), tmpPingTime.end());
    float max = *it;
    it = std::min_element(tmpPingTime.begin(), tmpPingTime.end());
    float min = *it;

    qreal sq_sum = std::inner_product(tmpPingTime.begin(), tmpPingTime.end(), tmpPingTime.begin(), 0.0);
    qreal stdev = 0.0;

    if (tmpPingTime.size() > 0)
    {
        avg /= tmpPingTime.size();
        // calculate standard deviation
        stdev = qSqrt(sq_sum / tmpPingTime.size() - avg * avg);
    }

    //res.append(roundTripMs);
    QVariantList resList;
    res.append(min);
    res.append(max);
    res.append(avg);
    res.append(stdev);
    res.append(m_pingsSent);
    res.append(m_pingsReceived);
    resList.append(QVariant(res));

    /*if (m_pingsSent > 0)
    {
        res.insert("round_trip_loss", (m_pingsSent - m_pingsReceived) / static_cast<float>(m_pingsSent));
    }
    else
    {
        res.insert("round_trip_loss", 0);
    }

    res.insert("destination_ip", m_destIp);*/

    return Result(resList);
}

quint32 Ping::estimateTraffic() const
{
    /*
     * Some headers appear in both the request and the response packet, hence
     * they are added twice to the estimation.
     */
    quint32 est = 2 * 14;  // Ethernet header

    switch (m_destAddress.sa.sa_family)
    {
    case AF_INET:
        est += 2 * 20;  // IPv4 header
        break;

    case AF_INET6:
        est += 2 * 40;  // IPv6 header
        break;
    }

    switch (definition->type)
    {
    case ping::Udp:
        est += 2 * (8 + definition->payload);  // UDP header + payload

        switch (m_destAddress.sa.sa_family)
        {
        case AF_INET:
            est += 36;  // ICMPv4 response
            break;

        case AF_INET6:
            est += 56;  // ICMPv6 response
            break;
        }

        break;

    case ping::Tcp:

        /*
         * TCP header bytes:
         *     SYN - RST/ACK:
         *         40 + 20 = 60 bytes (IPv4 and IPv6)
         *     SYN - SYN/ACK - ACK - RST/ACK:
         *         40 + 20 + 32 + 32 = 124 bytes (IPv4)
         *         40 + 40 + 32 + 32 = 144 bytes (IPv6)
         * Since there are 2 additional packets (SYN/ACK and ACK) when a
         * connect() succeeds, add
         *     2 * (12 + 20) bytes (IPv4).
         *     2 * (12 + 40) bytes (IPv6).
         * average:
         *     124 bytes (IPv4)
         *     154 bytes (IPv6)
         */
        switch (m_destAddress.sa.sa_family)
        {
        case AF_INET:
            est += 124;
            break;

        case AF_INET6:
            est += 154;
            break;
        }

        break;

    case ping::System:
        est += 40;  // ICMP header
        break;

    default:
        break;
    }

    est *= definition->count;

    return est;
}

int Ping::initSocket()
{
    int ttl = definition->ttl ? definition->ttl : 64;
    sockaddr_any src_addr;
    SOCKET sock = INVALID_SOCKET;
    struct linger sockLinger;

    memset(&src_addr, 0, sizeof(src_addr));
    memset(&sockLinger, 0, sizeof(sockLinger));

    sockLinger.l_linger = 0; //will make a call to close() send a RST
    sockLinger.l_onoff = 1;

    if (definition->type == ping::Udp)
    {
        sock = socket(m_destAddress.sa.sa_family, SOCK_DGRAM, IPPROTO_UDP);
    }
    else if (definition->type == ping::Tcp)
    {
        sock = socket(m_destAddress.sa.sa_family, SOCK_STREAM, IPPROTO_TCP);
    }

    if (sock == INVALID_SOCKET)
    {
        LOG_ERROR("invalid socket");
        return -1;
    }

    if (definition->type == ping::Tcp)
    {
        //this option will make TCP send a RST on close(), this way we do not run into
        //TIME_WAIT, which would make us not to being able to reuse the 5-tuple
        //for some time (we want to do that however for our Paris traceroute)
        if (setsockopt(sock, SOL_SOCKET, SO_LINGER, (char *)&sockLinger, sizeof(sockLinger)) < 0)
        {
            LOG_ERROR(QString("setsockopt SO_LINGER: %1").arg(
                          QString::fromLocal8Bit(strerror(errno))));
            goto cleanup;
        }

        //we need to make the TCP socket non-blocking, otherwise we might end up blocking
        //in connect later for a really really long time
        ulong flags = 1;
        flags = ioctlsocket(sock, FIONBIO, &flags);
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
        LOG_ERROR("bind: " + QString(strerror(errno)));
        goto cleanup;
    }

    // set TTL
    if (m_destAddress.sa.sa_family == AF_INET)
    {
        if (setsockopt(sock, IPPROTO_IP, IP_TTL, (char *) &ttl,
                       sizeof(ttl)) != 0)
        {
            LOG_ERROR("setsockopt IP_TTL: " + QString(strerror(errno)));
            goto cleanup;
        }
    }
    else if (m_destAddress.sa.sa_family == AF_INET6)
    {
        if (setsockopt(sock, IPPROTO_IPV6, IPV6_UNICAST_HOPS, (char *) &ttl,
                       sizeof(ttl)) != 0)
        {
            LOG_ERROR("setsockopt IPV6_UNICAST_HOPS: " +
                      QString(strerror(errno)));
            goto cleanup;
        }
    }

    return sock;

cleanup:
    closesocket(sock);
    return -1;
}

bool Ping::sendTcpData(PingProbe *probe)
{
    int ret = 0;

    if (m_destAddress.sa.sa_family == AF_INET)
    {
        ret = ::connect(probe->sock, (sockaddr *)&m_destAddress, sizeof(struct sockaddr_in));
    }
    else if (m_destAddress.sa.sa_family == AF_INET6)
    {
        ret = ::connect(probe->sock, (sockaddr *)&m_destAddress, sizeof(struct sockaddr_in6));
    }

    // check whether a connection could be established
    if (ret < 0 && WSAGetLastError() == WSAEWOULDBLOCK)
    {
        struct timeval tv = {0, 250000};
        fd_set fds;

        FD_ZERO(&fds);
        FD_SET(probe->sock, &fds);

        ret = select(1, NULL, &fds, NULL, &tv);

        if (ret >= 0)
        {
            // connection established -> re-initialize socket
            closesocket(probe->sock);
            probe->sock = initSocket();
        }
    }

    return true;
}

bool Ping::sendUdpData(PingProbe *probe)
{
    if (sendto(probe->sock, randomizePayload(definition->payload).data(),
               definition->payload, 0, (sockaddr *)&m_destAddress,
               sizeof(m_destAddress)) < 0)
    {
        LOG_WARNING("send: " + QString(strerror(errno)));
        return false;
    }

    return true;
}

void Ping::receiveData(PingProbe *probe)
{
    Q_UNUSED(probe);
}

void Ping::ping(PingProbe *probe)
{
    QVector<PingProbe> result;
    QFuture<QVector<PingProbe>> future;

    future = QtConcurrent::run(&receiveLoop, m_capture, m_destAddress,
                               definition->count * 2, definition->payload);

    if (definition->type == ping::Udp)
    {
        for (quint32 i = 0; i < definition->count; i++)
        {
            if (!sendUdpData(probe))
            {
                LOG_WARNING("error while sending");
            }

            m_pingsSent++;
            QThread::usleep(definition->interval * 1000);
        }
    }
    else if (definition->type == ping::Tcp)
    {
        for (quint32 i = 0; i < definition->count; i++)
        {
            if (!sendTcpData(probe))
            {
                LOG_WARNING("error while sending");
            }

            m_pingsSent++;
            QThread::usleep(definition->interval * 1000);
        }
    }

    future.waitForFinished();
    result = future.result();

    if (static_cast<quint32>(result.size()) < definition->count * 2)
    {
        LOG_WARNING("error while capturing packets");
        return;
    }

    if (definition->type == ping::Udp)
    {
        processUdpPackets(&result);
    }
    else if (definition->type == ping::Tcp)
    {
        processTcpPackets(&result);
    }

    foreach (const PingProbe &probe, m_pingProbes)
    {
        float pt = (probe.recvTime - probe.sendTime) / 1000.;

        if (pt >= definition->receiveTimeout)
        {
            if (m_pingsReceived > 0)
            {
                m_pingsReceived--;
            }

            continue;
        }

        if (probe.sendTime > 0 && probe.recvTime > 0 && probe.sendTime != probe.recvTime)
        {
            pingTime.append(pt);
        }
    }
}

void Ping::processUdpPackets(QVector<PingProbe> *probes)
{
    PingProbe newProbe;

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
    for (QVector<PingProbe>::iterator p = probes->begin(); p != probes->end(); p++)
    {
        // match a request packet ...
        if (p->marked || p->type != ping::UDP || p->response == ping::UDP_RESPONSE)
        {
            continue;
        }

        for (QVector<PingProbe>::iterator q = probes->begin(); q != probes->end(); q++)
        {
            // ... with a response packet
            if (q->marked || (q->type != ping::ICMP || p->hash != q->hash))
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
            case ping::DESTINATION_UNREACHABLE:
                m_pingsReceived++;
                emit destinationUnreachable(newProbe);
                break;

            case ping::TTL_EXCEEDED:
                m_pingsReceived++;
                emit ttlExceeded(newProbe);
                break;

            case ping::UNHANDLED_ICMP:
                LOG_WARNING("Unhandled ICMP packet (type/code): " +
                            QString::number(newProbe.icmpType) + "/" +
                            QString::number(newProbe.icmpCode));
                break;
            }

            break;
        }
    }

    // process possible UDP responses
    for (QVector<PingProbe>::iterator p = probes->begin(); p != probes->end(); p++)
    {
        if (p->marked || p->type != ping::UDP || p->response == ping::UDP_RESPONSE)
        {
            continue;
        }

        for (QVector<PingProbe>::iterator q = probes->begin(); q != probes->end(); q++)
        {
            if (!q->marked && (q->type == ping::UDP && q->response == ping::UDP_RESPONSE))
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

                m_pingsReceived++;
                emit udpResponse(newProbe);
                break;
            }
        }
    }
}

void Ping::processTcpPackets(QVector<PingProbe> *probes)
{
    PingProbe newProbe;

    // match TCP requests and responses first
    for (QVector<PingProbe>::iterator p = probes->begin(); p != probes->end(); p++)
    {
        if (p->marked || p->type != ping::TCP || (p->response == ping::TCP_CONNECT ||
                                                  p->response == ping::TCP_RESET))
        {
            continue;
        }

        for (QVector<PingProbe>::iterator q = probes->begin(); q != probes->end(); q++)
        {
            if (!q->marked && (q->type == ping::TCP && (q->response == ping::TCP_CONNECT ||
                                                        q->response == ping::TCP_RESET)))
            {
                p->marked = true;
                q->marked = true;

                newProbe.sendTime = p->sendTime;
                newProbe.recvTime = q->recvTime;
                newProbe.source = q->source;
                newProbe.response = q->response;

                m_pingProbes.append(newProbe);

                switch (newProbe.response)
                {
                case ping::TCP_CONNECT:
                    m_pingsReceived++;
                    emit tcpConnect(newProbe);
                    break;

                case ping::TCP_RESET:
                    m_pingsReceived++;
                    emit tcpReset(newProbe);
                    break;
                }

                break;
            }
        }
    }

    // match remaining TCP and ICMP packets
    for (QVector<PingProbe>::iterator p = probes->begin(); p != probes->end(); p++)
    {
        if (p->marked || p->type != ping::TCP || (p->response == ping::TCP_CONNECT ||
                                                  p->response == ping::TCP_RESET))
        {
            continue;
        }

        for (QVector<PingProbe>::iterator q = probes->begin(); q != probes->end(); q++)
        {
            if (!q->marked && q->type == ping::ICMP)
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

                switch (newProbe.response)
                {
                case ping::DESTINATION_UNREACHABLE:
                    m_pingsReceived++;
                    emit destinationUnreachable(newProbe);
                    break;

                case ping::TTL_EXCEEDED:
                    m_pingsReceived++;
                    emit ttlExceeded(newProbe);
                    break;

                case ping::UNHANDLED_ICMP:
                    LOG_WARNING("Unhandled ICMP packet (type/code): " +
                                QString::number(newProbe.icmpType) + "/" +
                                QString::number(newProbe.icmpCode));
                    break;
                }

                break;
            }
        }
    }
}

void Ping::started()
{
    pingTime.clear();

    setStatus(Ping::Running);
}

void Ping::finished(int exitCode, QProcess::ExitStatus exitStatus)
{
    Q_UNUSED(exitCode);
    Q_UNUSED(exitStatus);

    setStatus(Ping::Finished);
    emit Measurement::finished();
}

void Ping::readyRead()
{
    // Pinging google.com [173.194.113.5] with 32 bytes of data:
    QRegExp ip("\\[(.+)\\]");
    // Antwort von 193.99.144.80: Bytes=32 Zeit=32ms TTL=245
    QRegExp re("=(\\d+)ms");
    // Packets: Sent = 3, Received = 3, Lost = 0 (0% loss),
    QRegExp stats(".*:.*= (\\d+),.*= (\\d+),.*=.*\(.*%.*\),");

    for (QString line = stream.readLine(); !line.isNull(); line = stream.readLine())
    {
        if (ip.indexIn(line) > -1)
        {
            m_destIp = ip.cap(1);
        }

        if (stats.indexIn(line) > -1)
        {
            m_pingsSent = stats.cap(1).toUInt();
            m_pingsReceived = stats.cap(2).toUInt();
        }

        if (re.indexIn(line) == -1)
        {
            continue;
        }

        float time = re.cap(1).toFloat();
        pingTime.append(time);

        emit ping(time);
    }
}

void Ping::waitForFinished()
{
    process.waitForFinished(1000);
}

float Ping::averagePingTime() const
{
    float time = 0;

    foreach (float t, pingTime)
    {
        time += t;
    }

    return time / pingTime.size();
}

// vim: set sts=4 sw=4 et:
