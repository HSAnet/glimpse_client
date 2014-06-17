#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include <sys/time.h>
#include <poll.h>

#include "udpping.h"
#include "../../log/logger.h"

//#include <arpa/inet.h>

LOGGER(UdpPing);

namespace
{
    bool getAddress(const QString &address, sockaddr_any *addr)
    {
        struct addrinfo *rp = NULL, *result = NULL;

        if (getaddrinfo(address.toLatin1(), NULL, NULL, &result))
        {
            return false;
        }

        for (rp = result; rp && rp->ai_family != AF_INET &&
             rp->ai_family != AF_INET6;
             rp = rp->ai_next)
        {
        }

        if (!rp)
        {
            //being here means that neither a v4 nor a v6 address was found
            //does that make any sense? Could this happen?
            freeaddrinfo(result);
            return false;
        }

        memcpy(addr, rp->ai_addr, rp->ai_addrlen);

        freeaddrinfo(result);

        return true;
    }

    //for the Mac OS version of UDPPing, the ICMP socket is used only
    //to receive the incoming ICMP errors
    int initIcmpSocket(const sockaddr_any &m_destAddress)
    {
        int n = 0;
        int sock = 0;

        sock = socket(m_destAddress.sa.sa_family, SOCK_DGRAM, IPPROTO_ICMP);

        if (sock < 0)
        {
            //emit error("icmp socket: " + QString(strerror(errno)));
            return -1;
        }

        n = 1;

        if (setsockopt(sock, SOL_SOCKET, SO_TIMESTAMP, &n, sizeof(n)) < 0)
        {
            //emit error("icmp socket setsockopt SOL_TIMESTAMP: " + QString(strerror(errno)));
            goto cleanup;
        }

        return sock;

cleanup:
        close(sock);
        return -1;
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

bool UdpPing::prepare(NetworkManager *networkManager, const MeasurementDefinitionPtr &measurementDefinition)
{
    Q_UNUSED(networkManager);

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

    if (m_destAddress.sa.sa_family == AF_INET)
    {
        m_destAddress.sin.sin_port = htons(definition->destinationPort ? definition->destinationPort : 33434);
    }
    else if (m_destAddress.sa.sa_family == AF_INET6)
    {
        m_destAddress.sin6.sin6_port = htons(definition->destinationPort ? definition->destinationPort : 33434);
    }
    else
    {
        // unreachable
        return false;
    }

    return true;
}

bool UdpPing::start()
{
    PingProbe probe;

    // include null-character
    m_payload = new char[definition->payload + 1];

    memset(m_payload, 0, definition->payload + 1);

    setStartDateTime(QDateTime::currentDateTime());

    setStatus(UdpPing::Running);

    memset(&probe, 0, sizeof(probe));
    probe.sock = initSocket();

    if (probe.sock < 0)
    {
        emit error("socket: " + QString(strerror(errno)));
        delete[] m_payload;
        return false;
    }

    probe.icmpSock = initIcmpSocket(m_destAddress);

    if (probe.icmpSock < 0)
    {
        emit error(QString("icmp socket: %1").arg(QString::fromLocal8Bit(strerror(errno))));
        delete[] m_payload;
        return false;
    }

    for (quint32 i = 0; i < definition->count; i++)
    {
        ping(&probe);
        m_pingProbes.append(probe);
    }

    close(probe.sock);
    close(probe.icmpSock);

    setStatus(UdpPing::Finished);
    delete[] m_payload;
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
    int n = 0;
    int sock = 0;
    int ttl = definition->ttl ? definition->ttl : 64;
    sockaddr_any src_addr;

    memset(&src_addr, 0, sizeof(src_addr));

    sock = socket(m_destAddress.sa.sa_family, SOCK_DGRAM, IPPROTO_UDP);

    if (sock < 0)
    {
        emit error(QString("socket: %1").arg(QString::fromLocal8Bit(strerror(errno))));
        return -1;
    }

    if (m_destAddress.sa.sa_family == AF_INET)
    {
        src_addr.sin.sin_port = htons(definition->sourcePort);
        src_addr.sin.sin_family = AF_INET;
        src_addr.sin.sin_addr.s_addr = htonl(INADDR_ANY);

        if (bind(sock, (struct sockaddr *) &src_addr, sizeof(struct sockaddr_in)) < 0)
        {
            emit error(QString("bind: %1").arg(QString::fromLocal8Bit(strerror(errno))));
            goto cleanup;
        }
    }
    else if (m_destAddress.sa.sa_family == AF_INET6)
    {
        src_addr.sin6.sin6_port = htons(definition->sourcePort);
        src_addr.sin6.sin6_family = AF_INET6;
        src_addr.sin6.sin6_addr = in6addr_any;

        if (bind(sock, (struct sockaddr *) &src_addr, sizeof(struct sockaddr_in6)) < 0)
        {
            emit error(QString("bind: %1").arg(QString::fromLocal8Bit(strerror(errno))));
            goto cleanup;
        }
    }
    else
    {
        // unreachable
        goto cleanup;
    }

    n = 1;

    if (setsockopt(sock, SOL_SOCKET, SO_TIMESTAMP, &n, sizeof(n)) < 0)
    {
        emit error(QString("setsockopt SOL_TIMESTAMP: %1").arg(QString::fromLocal8Bit(strerror(errno))));
        goto cleanup;
    }

    // set TTL
    if (setsockopt(sock, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl)) < 0)
    {
        emit error(QString("setsockopt IP_TTL: %1").arg(QString::fromLocal8Bit(strerror(errno))));
        goto cleanup;
    }

    return sock;

cleanup:
    close(sock);
    return -1;
}

bool UdpPing::sendData(PingProbe *probe)
{
    struct timeval tv;

    memset(&tv, 0, sizeof(tv));
    gettimeofday(&tv, NULL);
    probe->sendTime = tv.tv_sec * 1e6 + tv.tv_usec;

    // randomize payload to prevent caching
    randomizePayload(m_payload, definition->payload);

    if (m_destAddress.sa.sa_family == AF_INET)
    {
        if (sendto(probe->sock, m_payload, definition->payload, 0,
                   (sockaddr *)&m_destAddress, sizeof(struct sockaddr_in)) < 0)
        {
            emit error(QString("send: %1").arg(QString::fromLocal8Bit(strerror(errno))));
            return false;
        }
    }
    else if (m_destAddress.sa.sa_family == AF_INET6)
    {
        if (sendto(probe->sock, m_payload, definition->payload, 0,
                   (sockaddr *)&m_destAddress, sizeof(struct sockaddr_in6)) < 0)
        {
            emit error(QString("send: %1").arg(QString::fromLocal8Bit(strerror(errno))));
            return false;
        }
    }
    else
    {
        //should never be reached
        return false;
    }

    return true;
}

void UdpPing::receiveData(PingProbe *probe)
{
    struct msghdr msg;
    sockaddr_any from;
    struct iovec iov;
    char buf[1280];
    char control[256];
    struct cmsghdr *cm;
    int ret = 0;
    bool udp_response = false;
    struct timeval tv;

    memset(&buf, 0, 1280);
    memset(&tv, 0, sizeof(tv));

    //will be overwritten if the packet was timestamped by the kernel
    //note: on Mavericks (10.9.3) the kernel did
    gettimeofday(&tv, NULL);
    probe->recvTime = tv.tv_sec * 1e6 + tv.tv_usec;

    msg.msg_name = &from;
    msg.msg_namelen = sizeof(from);
    msg.msg_control = control;
    msg.msg_controllen = sizeof(control);
    iov.iov_base = buf;
    iov.iov_len = sizeof(buf);
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;

    // poll here
    struct pollfd pfd[2];
    memset(&pfd, 0, sizeof(struct pollfd) * 2);
    pfd[0].fd = probe->sock;
    pfd[1].fd = probe->icmpSock;
    pfd[0].events = POLLIN;
    pfd[1].events = POLLIN;

    if ((ret = poll(pfd, 2, definition->receiveTimeout)) < 0)
    {
        emit error(QString("poll: %1").arg(QString::fromLocal8Bit(strerror(errno))));
        goto cleanup;
    }

    if (ret == 0)
    {
        emit timeout(*probe);
        goto cleanup;
    }

    //Which socket is ready to receive? Likely the ICMP socket
    if (pfd[1].revents & POLLIN)
    {
        //socket ready to receive
        if (recvmsg(probe->icmpSock, &msg, 0) < 0)
        {
            emit error(QString("recvmsg: %1").arg(QString::fromLocal8Bit(strerror(errno))));
            goto cleanup;
        }

        //we received an ICMP error... emit the right signal
        //so parse the header
        //get the Internet header length (in 32 bit quantaties)
        unsigned char ihl = buf[0] & 0x0F;

        if (ihl < 5 || ihl > 15)
        {
            emit error(QString("parsing icmp message failed (IHL value out of bounds)"));
            goto cleanup;
        }

        ihl *= 4;

        unsigned char icmp_type = 0;
        unsigned char icmp_code = 0;

        icmp_type = buf[ihl];
        icmp_code = buf[ihl + 1];

        if (icmp_type == 3 && icmp_code == 3)
        {
            emit destinationUnreachable(*probe);
        }
        else if (icmp_type == 11 && icmp_code == 0)
        {
            emit ttlExceeded(*probe);
        }

    }
    else if (pfd[0].revents & POLLIN)
    {
        //the UDP sender socket is ready to receive, there was indeed something
        //listening... how interesting (should be rare)
        if (recvmsg(probe->sock, &msg, 0) < 0)
        {
            emit error(QString("recvmsg: %1").arg(QString::fromLocal8Bit(strerror(errno))));
            goto cleanup;
        }

        udp_response = true;
    }
    else
    {
        //somthing else happened... not expected
        emit error(QString("recvmsg failed: No socket to read data from."));
        goto cleanup;
    }

    //not interested in the actual data at this point, only the sender
    memcpy(&probe->source, &from, sizeof(sockaddr_any));

    //need to parse the control data and search for the timestamp if available
    for (cm = CMSG_FIRSTHDR(&msg); cm; cm = CMSG_NXTHDR(&msg, cm))
    {
        void *ptr = CMSG_DATA(cm);

        if (cm->cmsg_level == SOL_SOCKET)
        {
            //the good folks at Apple decided to not use the standard names here
            //god bless their souls
            if (cm->cmsg_type == SCM_TIMESTAMP && cm->cmsg_len == CMSG_LEN(sizeof(struct timeval)))
            {
                struct timeval *tv_tmp = (struct timeval *) ptr;

                if (tv_tmp == NULL)
                {
                    break;
                }

                probe->recvTime = tv_tmp->tv_sec * 1e6 + tv_tmp->tv_usec;
                //we break after the first found timestamp, since on Mac OS there are
                //multiple (I assume for now these are for Ethernet, IP and UDP).
                //Tests showed they are increasing in the usec range whereas the first
                //seems deterministially the smallest
                break;
            }
        }
    }

    if (udp_response)
    {
        emit udpResponse(*probe);
    }

cleanup:
    return;
}

void UdpPing::ping(PingProbe *probe)
{
    // send
    if (sendData(probe))
    {
        receiveData(probe);
    }
}

// vim: set sts=4 sw=4 et:

