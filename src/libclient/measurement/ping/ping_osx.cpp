#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include <sys/time.h>
#include <poll.h>
#include <fcntl.h>
#include <numeric>

#include <QtMath>
#include <QThread>

#include "ping.h"
#include "../../log/logger.h"
#include "../../client.h"
#include "../../trafficbudgetmanager.h"

//#include <arpa/inet.h>

#ifdef Q_OS_IOS
#include <unistd.h>
#endif

LOGGER(Ping);

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

    inline bool extractTimestamp(struct msghdr &msg, quint64 &time)
    {
        struct cmsghdr *cm = NULL;

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
                        return false;
                    }

                    time = tv_tmp->tv_sec * 1e6 + tv_tmp->tv_usec;
                    //we break after the first found timestamp, since on Mac OS there are
                    //multiple (I assume for now these are for Ethernet, IP and UDP).
                    //Tests showed they are increasing in the usec range whereas the first
                    //seems deterministially the smallest
                    return true;
                }
            }
        }

        return false;
    }
}

Ping::Ping(QObject *parent)
: Measurement(parent)
, currentStatus(Unknown)
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

    definition = measurementDefinition.dynamicCast<PingDefinition>();

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
        setErrorString(QString("unknown address family '%1'").arg(
                           m_destAddress.sa.sa_family));
        return false;
    }

    return true;
}

bool Ping::start()
{
    PingProbe probe;

    setStatus(Ping::Running);

    if (definition->type == ping::System)
    {
        QStringList args;

        args << "-c" << QString::number(definition->count)
             << "-W" << QString::number(definition->receiveTimeout)
             << "-i" << QString::number((float)definition->interval / 1000)
             << definition->host;

        process.kill();
        process.start("ping", args);

        return true;
    }

    probe.sock = initSocket();

    if (probe.sock < 0)
    {
        setErrorString(QString("socket: %1").arg(QString::fromLocal8Bit(strerror(errno))));
        return false;
    }

    probe.icmpSock = initIcmpSocket(m_destAddress);

    if (probe.icmpSock < 0)
    {
        close(probe.sock);
        setErrorString(QString("icmp socket: %1").arg(QString::fromLocal8Bit(strerror(errno))));
        return false;
    }

    for (quint32 i = 0; i < definition->count; i++)
    {
        ping(&probe);
        m_pingProbes.append(probe);
        QThread::msleep(definition->interval);
    }

    close(probe.sock);
    close(probe.icmpSock);

    foreach (const PingProbe &probe, m_pingProbes)
    {
        if (probe.sendTime > 0 && probe.recvTime > 0 && probe.sendTime != probe.recvTime)
        {
            pingTime.append((probe.recvTime - probe.sendTime) / 1000.);
        }
    }

    setStatus(Ping::Finished);
    emit Measurement::finished();

    return true;
}

bool Ping::stop()
{
    return true;
}

Result Ping::result() const
{
    QVariantMap res;
    QVariantList roundTripMs;
    float avg = 0.0;

    // get max and min
    QList<float>::const_iterator it = std::max_element(pingTime.begin(), pingTime.end());
    float max = *it;
    it = std::min_element(pingTime.begin(), pingTime.end());
    float min = *it;

    // calculate average and fill ping times
    foreach (float val, pingTime)
    {
        roundTripMs << val;
        avg += val;
    }

    qreal sq_sum = std::inner_product(pingTime.begin(), pingTime.end(), pingTime.begin(), 0.0);
    qreal stdev = 0.0;

    if (pingTime.size() > 0)
    {
        avg /= pingTime.size();
        // calculate standard deviation
        stdev = qSqrt(sq_sum / pingTime.size() - avg * avg);
    }

    res.insert("round_trip_avg", avg);
    res.insert("round_trip_min", min);
    res.insert("round_trip_max", max);
    res.insert("round_trip_stdev", stdev);
    res.insert("round_trip_count", pingTime.size());
    res.insert("round_trip_ms", roundTripMs);

    return Result(res);
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
        est += 64;  // ICMP header
        break;

    default:
        break;
    }

    est *= definition->count;

    return est;
}

int Ping::initSocket()
{
    int n = 0;
    int sock = 0;
    int ttl = definition->ttl ? definition->ttl : 64;
    sockaddr_any src_addr;
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
    //this should never happen
    else
    {
        LOG_ERROR(QString("init socket: unkown socket type"));
        return -1;
    }

    if (sock < 0)
    {
        LOG_ERROR(QString("socket: %1").arg(QString::fromLocal8Bit(strerror(errno))));
        return -1;
    }

    if (definition->type == ping::Tcp)
    {
        //this option will make TCP send a RST on close(), this way we do not run into
        //TIME_WAIT, which would make us not to being able to reuse the 5-tuple
        //for some time (we want to do that howerver for our Paris traceroute
        if (setsockopt(sock, SOL_SOCKET, SO_LINGER, &sockLinger, sizeof(sockLinger)) < 0)
        {
            LOG_ERROR(QString("setsockopt SO_LINGER: %1").arg(QString::fromLocal8Bit(strerror(errno))));
            goto cleanup;
        }

        //we need to make the TCP socket non-blocking, otherwise we might end up blocking
        //in connect later for a really really long time
        int flags = 0;
        flags = fcntl(sock, F_GETFL, 0);

        if (fcntl(sock, F_SETFL, flags | O_NONBLOCK) < 0)
        {
            LOG_WARNING(QString("making TCP socket non-blocking : %1").arg(QString::fromLocal8Bit(strerror(errno))));
            //we don't go to clean-up here since the code will still function... there just might
            //be a potential longish wait
        }
    }

    if (m_destAddress.sa.sa_family == AF_INET)
    {
        src_addr.sin.sin_port = htons(definition->sourcePort);
        src_addr.sin.sin_family = AF_INET;
        src_addr.sin.sin_addr.s_addr = htonl(INADDR_ANY);

        if (bind(sock, (struct sockaddr *) &src_addr, sizeof(struct sockaddr_in)) < 0)
        {
            LOG_ERROR(QString("bind: %1").arg(QString::fromLocal8Bit(strerror(errno))));
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
            LOG_ERROR(QString("bind: %1").arg(QString::fromLocal8Bit(strerror(errno))));
            goto cleanup;
        }
    }
    else
    {
        // unreachable
        LOG_ERROR(QString("init socket: unknown address family"));
        goto cleanup;
    }

    n = 1;

    if (setsockopt(sock, SOL_SOCKET, SO_TIMESTAMP, &n, sizeof(n)) < 0)
    {
        LOG_ERROR(QString("setsockopt SOL_TIMESTAMP: %1").arg(QString::fromLocal8Bit(strerror(errno))));
        goto cleanup;
    }

    // set TTL
    if (setsockopt(sock, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl)) < 0)
    {
        LOG_ERROR(QString("setsockopt IP_TTL: %1").arg(QString::fromLocal8Bit(strerror(errno))));
        goto cleanup;
    }

    return sock;

cleanup:
    close(sock);
    return -1;
}

bool Ping::sendUdpData(PingProbe *probe)
{
    struct timeval tv;

    memset(&tv, 0, sizeof(tv));

    gettimeofday(&tv, NULL);
    probe->sendTime = tv.tv_sec * 1e6 + tv.tv_usec;

    if (m_destAddress.sa.sa_family == AF_INET)
    {
        if (sendto(probe->sock, randomizePayload(definition->payload).data(),
                   definition->payload, 0, (sockaddr *)&m_destAddress,
                   sizeof(struct sockaddr_in)) < 0)
        {
            LOG_WARNING(QString("send: %1").arg(QString::fromLocal8Bit(strerror(errno))));
            return false;
        }
    }
    else if (m_destAddress.sa.sa_family == AF_INET6)
    {
        if (sendto(probe->sock, randomizePayload(definition->payload).data(),
                   definition->payload, 0, (sockaddr *)&m_destAddress,
                   sizeof(struct sockaddr_in6)) < 0)
        {
            LOG_WARNING(QString("send v6: %1").arg(QString::fromLocal8Bit(strerror(errno))));
            return false;
        }
    }
    else
    {
        //should never be reached
        LOG_WARNING(QString("sendUdpData: unkown address family"));
        return false;
    }

    return true;
}

bool Ping::sendTcpData(PingProbe *probe)
{
    struct timeval tv;

    memset(&tv, 0, sizeof(tv));

    int ret = 0;

    gettimeofday(&tv, NULL);
    probe->sendTime = tv.tv_sec * 1e6 + tv.tv_usec;

    if (m_destAddress.sa.sa_family == AF_INET)
    {
        ret = ::connect(probe->sock, (sockaddr *)&m_destAddress, sizeof(struct sockaddr_in));
    }
    else if (m_destAddress.sa.sa_family == AF_INET6)
    {
        ret = ::connect(probe->sock, (sockaddr *)&m_destAddress, sizeof(struct sockaddr_in6));
    }
    else
    {
        LOG_WARNING(QString("connect: unknown address family"));
        return false;
    }

    gettimeofday(&tv, NULL);
    probe->recvTime = tv.tv_sec * 1e6 + tv.tv_usec;
    memcpy(&probe->source, &(m_destAddress), sizeof(sockaddr_any));

    //for a TCP socket we only call connect (remeber, the socket is non-blocking)
    if (ret < 0)
    {
        if (errno == ECONNRESET || errno == ECONNREFUSED)
        {
            //check immediately, as connect could return immediately if called
            //for host-local addresses
            emit tcpReset(*probe);
            goto tcpcleanup;
        }
        else if (errno == EINPROGRESS)
        {
            //the call to connect came right back, we now wait a little
            //till it goes through or till we receive a reset
            struct pollfd pfd;
            int ret = 0;

            memset(&pfd, 0, sizeof(pfd));
            pfd.fd = probe->sock;
            pfd.events = POLLIN | POLLOUT | POLLERR;

            //careful: when an error occured, the socket becomes also writeable
            //i.e. POLLOUT.
            if ((ret = poll(&pfd, 1, definition->receiveTimeout)) < 0)
            {
                LOG_WARNING(QString("poll: %1").arg(QString::fromLocal8Bit(strerror(errno))));
                goto tcpcleanup;
            }

            //timeout... the other end is not responding...
            //maybe a stupid Windows host or something
            //other went wrong (ICMP)... need to distinguish
            //the two
            if (ret == 0)
            {
                //it seems counter intuitive, but we
                //return true here, so we actually
                //try to read from the ICMP socket...
                close(probe->sock);
                probe->sock = initSocket();

                if (probe->sock < 0)
                {
                    LOG_WARNING(QString("socket: %1").arg(QString::fromLocal8Bit(strerror(errno))));
                    return false;
                }

                //receiveData will return the right signal for us
                return true;
            }

            //the call to connect could have failed (RST) or actually went through
            //need to check
            int error_num = 0;
            socklen_t len = sizeof(error_num);

            if (getsockopt(probe->sock, SOL_SOCKET, SO_ERROR, &error_num, &len) < 0)
            {
                LOG_WARNING(QString("getsockopt: %1").arg(QString::fromLocal8Bit(strerror(error_num))));
                goto tcpcleanup;
            }

            gettimeofday(&tv, NULL);
            probe->recvTime = tv.tv_sec * 1e6 + tv.tv_usec;
            memcpy(&probe->source, &(m_destAddress), sizeof(sockaddr_any));

            if (error_num == 0)
            {
                //connection established
                emit tcpConnect(*probe);
                goto tcpcleanup;
            }
            else if (error_num == ECONNRESET || error_num == ECONNREFUSED)
            {
                //we really expected this reset...
                emit tcpReset(*probe);
                goto tcpcleanup;
            }
            else
            {
                //unexpected
                LOG_WARNING(QString("getsockopt: %1").arg(QString::fromLocal8Bit(strerror(error_num))));
                goto tcpcleanup;
            }
        }
        else
        {
            //unexpected
            LOG_WARNING(QString("connect: %1").arg(QString::fromLocal8Bit(strerror(errno))));
            goto tcpcleanup;
        }
    }

    //when we are here, the 3-way handshake went through...
    //really, really fast which could happen if the destination is host local

    emit tcpConnect(*probe);

tcpcleanup:

    //closing after unsuccessful connect() OK
    close(probe->sock);
    probe->sock = initSocket();

    if (probe->sock < 0)
    {
        LOG_WARNING(QString("socket: %1").arg(QString::fromLocal8Bit(strerror(errno))));
    }

    //we return false to not enter receiveData() in ping()
    //which needs to be avoided for both a successful and
    //unsuccessful connect()
    return false;
}

void Ping::receiveData(PingProbe *probe)
{
    struct msghdr msg;
    sockaddr_any from;
    struct iovec iov;
    char buf[1280];
    char control[256];
    int ret = 0;
    struct timeval tv;

    memset(&buf, 0, 1280);
    memset(&tv, 0, sizeof(tv));

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

    // Don't poll on TCP sockets since the sendTcpData method already does
    // that.
    if (definition->type == ping::Udp)
    {
        if ((ret = poll(pfd, 2, definition->receiveTimeout)) < 0)
        {
            LOG_WARNING(QString("poll: %1").arg(QString::fromLocal8Bit(strerror(errno))));
            return;
        }
    }

    //will be overwritten if the packet was timestamped by the kernel
    //note: on Mavericks (10.9.3) the kernel did
    gettimeofday(&tv, NULL);
    probe->recvTime = tv.tv_sec * 1e6 + tv.tv_usec;

    if (ret == 0)
    {
        memcpy(&probe->source, &(m_destAddress), sizeof(sockaddr_any));
        // indicate a timeout by zeroing the ping duration
        probe->recvTime = probe->sendTime;
        emit timeout(*probe);
        return;
    }

    //Which socket is ready to receive? Likely the ICMP socket
    if (pfd[1].revents & POLLIN)
    {
        //socket ready to receive
        if (recvmsg(probe->icmpSock, &msg, 0) < 0)
        {
            LOG_WARNING(QString("recvmsg: %1").arg(QString::fromLocal8Bit(strerror(errno))));
            return;
        }

        //we received an ICMP error... emit the right signal
        //so parse the header
        //get the Internet header length (in 32 bit quantaties)
        unsigned char ihl = buf[0] & 0x0F;

        if (ihl < 5 || ihl > 15)
        {
            LOG_WARNING(QString("parsing icmp message failed (IHL value out of bounds)"));
            return;
        }

        ihl *= 4;

        unsigned char icmp_type = 0;
        unsigned char icmp_code = 0;

        icmp_type = buf[ihl];
        icmp_code = buf[ihl + 1];

        extractTimestamp(msg, probe->recvTime);
        memcpy(&probe->source, &from, sizeof(sockaddr_any));

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
        //the UDP/TCP sender socket is ready to receive, there was indeed something
        //listening... how interesting (should be rare)
        if (recvmsg(probe->sock, &msg, 0) < 0)
        {
            LOG_WARNING(QString("recvmsg: %1").arg(QString::fromLocal8Bit(strerror(errno))));
            return;
        }

        extractTimestamp(msg, probe->recvTime);
        memcpy(&probe->source, &from, sizeof(sockaddr_any));

        emit udpResponse(*probe);
    }
    else
    {
        //somthing else happened... not expected
        LOG_WARNING(QString("recvmsg failed: No socket to read data from."));
        return;
    }

    return;
}

void Ping::ping(PingProbe *probe)
{
    bool ret = true;

    // send
    if (definition->type == ping::Udp)
    {
        ret = sendUdpData(probe);
    }
    else if (definition->type == ping::Tcp)
    {
        ret = sendTcpData(probe);
    }
    else
    {
        LOG_WARNING(QString("ping: unkown socket type"));
    }

    if (ret)
    {
        receiveData(probe);
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
    // 64 bytes from 193.99.144.80: icmp_seq=0 ttl=245 time=32.031 ms
    QRegExp re("time=(\\d+.*)ms");

    for (QString line = stream.readLine(); !line.isNull(); line = stream.readLine())
    {
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
