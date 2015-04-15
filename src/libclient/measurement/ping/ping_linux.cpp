#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <poll.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <linux/errqueue.h>
#include <linux/icmp.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <numeric>

#include <QtMath>
#include <QThread>

#include "ping.h"
#include "../../log/logger.h"
#include "../../client.h"
#include "../../trafficbudgetmanager.h"

LOGGER("Ping");

namespace
{
    bool getAddress(const QString &address, sockaddr_any *addr)
    {
        struct addrinfo hints;
        struct addrinfo *rp = NULL, *result = NULL;

        memset(&hints, 0, sizeof(hints));
        hints.ai_family = PF_UNSPEC;

        if (getaddrinfo(address.toLatin1(), NULL, &hints, &result))
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
            freeaddrinfo(result);
            return false;
        }

        memcpy(addr, rp->ai_addr, rp->ai_addrlen);

        freeaddrinfo(result);

        return true;
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
             << "-n" // Don't resolve hostnames
             << "-W" << QString::number((float)definition->receiveTimeout / 1000)
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

    for (quint32 i = 0; i < definition->count; i++)
    {
        ping(&probe);
        m_pingProbes.append(probe);
        QThread::msleep(definition->interval);
    }

    close(probe.sock);

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
    if (definition->type == ping::System)
    {
        process.kill();
    }

    return true;
}

Result Ping::result() const
{
    QVariantMap res;
    QVariantList roundTripMs;
    float avg = 0.0;
    QList<float> tmpPingTime = pingTime;

    // calculate average and fill ping times
    foreach (float val, tmpPingTime)
    {
        // RTTs greater than 60s are discarded and don't influence the result
        if (val >= 60000.)
        {
            if (m_pingsReceived > 0)
            {
                m_pingsReceived--;
            }

            if (m_pingsSent > 0)
            {
                m_pingsSent--;
            }

            continue;
        }

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

    res.insert("round_trip_avg", avg);
    res.insert("round_trip_min", min);
    res.insert("round_trip_max", max);
    res.insert("round_trip_stdev", stdev);
    res.insert("round_trip_ms", roundTripMs);
    res.insert("round_trip_sent", m_pingsSent);  // count successfull pings only
    res.insert("round_trip_received", m_pingsReceived);

    if (m_pingsSent > 0)
    {
        res.insert("round_trip_loss", (m_pingsSent - m_pingsReceived) / m_pingsSent);
    }
    else
    {
        res.insert("round_trip_loss", 0);
    }

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
    else
    {
        // this should never happen
        LOG_ERROR(QString("Unknown ping type '%1'").arg(definition->type));
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
            LOG_ERROR(QString("setsockopt SO_LINGER: %1").arg(
                          QString::fromLocal8Bit(strerror(errno))));
            goto cleanup;
        }

        //we need to make the TCP socket non-blocking, otherwise we might end up blocking
        //in connect later for a really really long time
        int flags = 0;
        flags = fcntl(sock, F_GETFL, 0);

        if (fcntl(sock, F_SETFL, flags | O_NONBLOCK) < 0)
        {
            LOG_ERROR(QString("making TCP socket non-blocking : %1").arg(
                          QString::fromLocal8Bit(strerror(errno))));
            //we don't go to clean-up here since the code will still function... there just might
            //be a potential longish wait
        }
    }

    src_addr.sa.sa_family = m_destAddress.sa.sa_family;

    // use RECVRR
    n = 1;

    if (m_destAddress.sa.sa_family == AF_INET)
    {
        src_addr.sin.sin_port = htons(definition->sourcePort);

        if (bind(sock, (struct sockaddr *) &src_addr, sizeof(src_addr.sin)) < 0)
        {
            LOG_ERROR(QString("bind: %1").arg(QString::fromLocal8Bit(strerror(errno))));
            goto cleanup;
        }

        if (setsockopt(sock, SOL_IP, IP_RECVERR, &n, sizeof(n)) < 0)
        {
            LOG_ERROR(QString("setsockopt IP_RECVERR: %1").arg(
                          QString::fromLocal8Bit(strerror(errno))));
            goto cleanup;
        }
    }
    else if (m_destAddress.sa.sa_family == AF_INET6)
    {
        src_addr.sin6.sin6_port = htons(definition->sourcePort);

        if (bind(sock, (struct sockaddr *) &src_addr, sizeof(src_addr.sin6)) < 0)
        {
            LOG_ERROR(QString("bind: %1").arg(QString::fromLocal8Bit(strerror(errno))));
            goto cleanup;
        }

        if (setsockopt(sock, IPPROTO_IPV6, IPV6_RECVERR, &n, sizeof(n)) < 0)
        {
            LOG_ERROR(QString("setsockopt IPV6_RECVERR: %1").arg(
                          QString::fromLocal8Bit(strerror(errno))));
            goto cleanup;
        }
    }

    // Enable the receiving of the SO_TIMESTAMP control message
    n = 1;

    if (setsockopt(sock, SOL_SOCKET, SO_TIMESTAMP, &n, sizeof(n)) < 0)
    {
        LOG_ERROR(QString("setsockopt SOL_TIMESTAMP: %1").arg(
                      QString::fromLocal8Bit(strerror(errno))));
        goto cleanup;
    }

    // set TTL
    if (setsockopt(sock, SOL_IP, IP_TTL, &ttl, sizeof(ttl)) < 0)
    {
        LOG_ERROR(QString("setsockopt IP_TTL: %1").arg(
                      QString::fromLocal8Bit(strerror(errno))));
        goto cleanup;
    }

    return sock;

cleanup:
    close(sock);
    return -1;
}

bool Ping::sendUdpData(PingProbe *probe)
{
    int ret = 0;
    struct timeval tv;

    memset(&tv, 0, sizeof(tv));

    gettimeofday(&tv, NULL);
    probe->sendTime = tv.tv_sec * 1e6 + tv.tv_usec;

    if (m_destAddress.sa.sa_family == AF_INET)
    {
        ret = sendto(probe->sock, randomizePayload(definition->payload).data(),
                     definition->payload, 0, (sockaddr *)&m_destAddress,
                     sizeof(m_destAddress.sin));
    }
    else if (m_destAddress.sa.sa_family == AF_INET6)
    {
        ret = sendto(probe->sock, randomizePayload(definition->payload).data(),
                     definition->payload, 0, (sockaddr *)&m_destAddress,
                     sizeof(m_destAddress.sin6));
    }

    if (ret < 0)
    {
        LOG_WARNING(QString("send: %1").arg(QString::fromLocal8Bit(strerror(errno))));
        return false;
    }

    return true;
}

bool Ping::sendTcpData(PingProbe *probe)
{
    int ret = 0;
    struct timeval tv;

    memset(&tv, 0, sizeof(tv));
    gettimeofday(&tv, NULL);
    probe->sendTime = tv.tv_sec * 1e6 + tv.tv_usec;

    if (m_destAddress.sa.sa_family == AF_INET)
    {
        ret = ::connect(probe->sock, (sockaddr *)&m_destAddress,
                        sizeof(struct sockaddr_in));
    }
    else if (m_destAddress.sa.sa_family == AF_INET6)
    {
        ret = ::connect(probe->sock, (sockaddr *)&m_destAddress,
                        sizeof(struct sockaddr_in6));
    }

    gettimeofday(&tv, NULL);
    probe->recvTime = tv.tv_sec * 1e6 + tv.tv_usec;
    memcpy(&probe->source, &(m_destAddress), sizeof(sockaddr_any));

    //for a TCP socket we only call connect (remeber, the socket is non-blocking)
    if (ret < 0)
    {
        if (errno == ECONNRESET || errno == ECONNREFUSED)
        {
            m_pingsSent++;
            m_pingsReceived++;
            //check immediately, as connect could return immediately if called
            //for host-local addresses
            emit tcpReset(*probe);
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
                LOG_WARNING(QString("poll: %1").arg(QString::fromLocal8Bit(
                                                        strerror(errno))));
                goto tcpcleanup;
            }

            gettimeofday(&tv, NULL);
            probe->recvTime = tv.tv_sec * 1e6 + tv.tv_usec;
            memcpy(&probe->source, &(m_destAddress), sizeof(sockaddr_any));

            //timeout... the other end is not responding...
            //maybe a stupid Windows host or something
            //other went wrong (ICMP)... need to distinguish
            //the two
            if (ret == 0)
            {
                //it seems counter intuitive, but we
                //return true here, so we actually
                //try to read from the socket...
                return true;
            }

            m_pingsSent++;

            //the call to connect could have failed (RST) or actually went through
            //need to check
            int error_num = 0;
            socklen_t len = sizeof(error_num);

            if (getsockopt(probe->sock, SOL_SOCKET, SO_ERROR, &error_num, &len) < 0)
            {
                LOG_WARNING(QString("getsockopt: %1").arg(QString::fromLocal8Bit(
                                                              strerror(error_num))));
                goto tcpcleanup;
            }


            if (error_num == 0)
            {
                //connection established
                m_pingsReceived++;
                emit tcpConnect(*probe);
                goto tcpcleanup;
            }
            else if (error_num == ECONNRESET || error_num == ECONNREFUSED)
            {
                //we really expected this reset...
                m_pingsReceived++;
                emit tcpReset(*probe);
                goto tcpcleanup;
            }
            else
            {
                //unexpected
                LOG_WARNING(QString("getsockopt: %1").arg(QString::fromLocal8Bit(
                                                              strerror(error_num))));
                goto tcpcleanup;
            }
        }
        else
        {
            //unexpected
            LOG_WARNING(QString("connect: %1").arg(QString::fromLocal8Bit(
                                                       strerror(errno))));
            goto tcpcleanup;
        }
    }

tcpcleanup:
    //closing after unsuccessful connect() OK
    close(probe->sock);
    probe->sock = initSocket();

    if (probe->sock < 0)
    {
        LOG_WARNING(QString("socket: %1").arg(QString::fromLocal8Bit(strerror(errno))));
    }

    return false;
}

void Ping::receiveData(PingProbe *probe)
{
    struct msghdr msg;
    sockaddr_any from;
    struct iovec iov;
    char buf[1500];
    char control[256];
    struct cmsghdr *cm;
    struct sock_extended_err *ee = NULL;
    struct timeval *tv;
    bool udp_response = false;

    memset(&msg, 0, sizeof(msg));
    msg.msg_name = &from;
    msg.msg_namelen = sizeof(from);
    msg.msg_control = control;
    msg.msg_controllen = sizeof(control);
    iov.iov_base = buf;
    iov.iov_len = sizeof(buf);
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;

    // poll here
    struct pollfd pfd;
    memset(&pfd, 0, sizeof(pfd));
    pfd.fd = probe->sock;
    pfd.events = POLLIN | POLLERR;

    // Don't poll on TCP sockets since the sendTcpData method already does
    // that.
    if (definition->type == ping::Udp)
    {
        if (poll(&pfd, 1, definition->receiveTimeout) < 0)
        {
            LOG_WARNING(QString("poll: %1").arg(QString::fromLocal8Bit(strerror(errno))));
            goto cleanup;
        }
    }

    if (!pfd.revents)
    {
        // indicate a timeout by zeroing the ping duration
        probe->recvTime = probe->sendTime;
        emit timeout(*probe);

        if (definition->type == ping::Tcp)
        {
            close(probe->sock);
            probe->sock = initSocket();
        }

        goto cleanup;
    }

    if (recvmsg(probe->sock, &msg, MSG_ERRQUEUE) < 0)
    {
        // receive UDP packet
        if (recvmsg(probe->sock, &msg, 0) < 0)
        {
            LOG_WARNING(QString("recvmsg: %1").arg(QString::fromLocal8Bit(strerror(errno))));
            goto cleanup;
        }

        udp_response = true;

        // msg_name provides the source address if the initial request packet
        // was successful
        memcpy(&probe->source, &from, sizeof(sockaddr_any));
    }

    if (msg.msg_flags & MSG_CTRUNC)
    {
        LOG_WARNING("control message buffer is too short to store all messages");
    }

    for (cm = CMSG_FIRSTHDR(&msg); cm; cm = CMSG_NXTHDR(&msg, cm))
    {
        void *ptr = CMSG_DATA(cm);

        switch (cm->cmsg_level)
        {
        case SOL_SOCKET:
            switch (cm->cmsg_type)
            {
            case SO_TIMESTAMP:
                tv = (struct timeval *) ptr;
                probe->recvTime = tv->tv_sec * 1e6 + tv->tv_usec;
                break;
            }

            break;

        case SOL_IP:
            switch (cm->cmsg_type)
            {
            case IP_RECVERR:
                ee = (struct sock_extended_err *) ptr;

                if (ee->ee_origin != SO_EE_ORIGIN_ICMP)
                {
                    ee = NULL;
                    continue;
                }

                if (ee->ee_type == ICMP_SOURCE_QUENCH || ee->ee_type == ICMP_REDIRECT)
                {
                    goto cleanup;
                }

                break;

            default:
                break;
            }

            break;

        default:
            break;
        }
    }

    if (ee)
    {
        memcpy(&probe->source, SO_EE_OFFENDER(ee), sizeof(probe->source));

        if (ee->ee_type == ICMP_TIME_EXCEEDED && ee->ee_code == ICMP_EXC_TTL)
        {
            m_pingsReceived++;
            emit ttlExceeded(*probe);
        }
        else if (ee->ee_type == ICMP_DEST_UNREACH)
        {
            m_pingsReceived++;
            emit destinationUnreachable(*probe);
        }
    }

    if (udp_response)
    {
        m_pingsReceived++;
        emit udpResponse(*probe);
    }

cleanup:
    return;
}

void Ping::ping(PingProbe *probe)
{
    bool ret = false;

    // send
    if (definition->type == ping::Udp)
    {
        ret = sendUdpData(probe);
    }
    else if (definition->type == ping::Tcp)
    {
        ret = sendTcpData(probe);
    }

    if (ret)
    {
        m_pingsSent++;
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
    // 3 packets transmitted, 3 received, 0% packet loss, time 2002ms
    QRegExp stats("(\\d+).*, (\\d+).*,.*%.*,.*");

    for (QString line = stream.readLine(); !line.isNull(); line = stream.readLine())
    {
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
