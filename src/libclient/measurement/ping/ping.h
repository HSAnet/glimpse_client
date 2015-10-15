#ifndef PING_H
#define PING_H

#include <QtGlobal>
#include <QString>
#include <QVector>
#include <QProcess>
#include <QList>

#if defined(Q_OS_WIN)
#include <QtConcurrent/QtConcurrentRun>
#include <QMutex>
#include <winsock2.h>
#include <Ws2ipdef.h>
#include <WS2tcpip.h>
#undef min
#include<pcap.h>
#include <cstring>
#elif defined(Q_OS_LINUX) || defined(Q_OS_MAC)
#include <netinet/in.h>
#else
#error Platform not supported.
#endif

#include "../measurement.h"
#include "ping_definition.h"

union sockaddr_any
{
    struct sockaddr sa;
    struct sockaddr_in sin;
    struct sockaddr_in6 sin6;
};

/*
 * This enum exists because receiveLoop() cannot directly emit signals.
 * It's meant to notify ping() about how things are to be handled.
 */
#if defined(Q_OS_WIN)
namespace ping
{
    enum Response
    {
        INVALID_ICMP = 1,
        DESTINATION_UNREACHABLE,
        TTL_EXCEEDED,
        UNHANDLED_ICMP,
        UDP_RESPONSE,
        TCP_RESET,
        TCP_CONNECT
    };

    enum PacketType
    {
        UNKNOWN,
        UDP,
        ICMP,
        TCP
    };
}
#else
typedef void pcap_if_t;
typedef void pcap_t;
#endif

struct PingProbe
{
    int sock;
    quint64 sendTime;
    quint64 recvTime;
    sockaddr_any source;
#if defined(Q_OS_WIN)
    ping::PacketType type;
    ping::Response response;
    quint8 icmpType;
    quint8 icmpCode;
    QByteArray hash;
    bool marked;

    bool operator==(const PingProbe &p) const
    {
        return !memcmp(&source, &p.source, sizeof(source)) &&
               type == p.type &&
               response == p.response &&
               icmpType == p.icmpType &&
               icmpCode == p.icmpCode &&
               hash == p.hash;
    }

#endif
#if defined(Q_OS_MAC)
    int icmpSock;
#endif

    PingProbe()
    : sock(0)
    , sendTime(0)
    , recvTime(0)
    , source()
#if defined(Q_OS_WIN)
    , type()
    , response()
    , icmpType(0)
    , icmpCode(0)
    , marked(false)
#endif
#if defined(Q_OS_MAC)
    , icmpSock(0)
#endif
    {}
};

class Ping : public Measurement
{
    Q_OBJECT

public:

    explicit Ping(QObject *parent = 0);
    ~Ping();

    // Measurement interface
    Status status() const;
    bool prepare(NetworkManager *networkManager, const MeasurementDefinitionPtr &measurementDefinition);
    bool start();
    bool stop();
    Result result() const;
    void waitForFinished();
    float averagePingTime() const;

private:
    quint32 estimateTraffic() const;
    void setStatus(Status status);
    int initSocket();
    bool sendUdpData(PingProbe *probe);
    bool sendTcpData(PingProbe *probe);
    void receiveData(PingProbe *probe);
    void ping(PingProbe *probe);
#if defined(Q_OS_WIN)
    void processUdpPackets(QVector<PingProbe> *probes);
    void processTcpPackets(QVector<PingProbe> *probes);
#endif

    PingDefinitionPtr definition;
    Status currentStatus;
    QVector<PingProbe> m_pingProbes;
    QList<float> pingTime;
    quint32 m_pingsSent;
    quint32 m_pingsReceived;
    QString m_destIp;

    pcap_if_t *m_device;
    pcap_t *m_capture;
    sockaddr_any m_destAddress;

    // for system ping only
    QProcess process;
    QTextStream stream;

signals:
    void statusChanged(Status status);
    void destinationUnreachable(const PingProbe &probe);
    void ttlExceeded(const PingProbe &probe);
    void udpResponse(const PingProbe &probe);
    void timeout(const PingProbe &probe);
    void tcpReset(const PingProbe &probe);
    void tcpConnect(const PingProbe &probe);
    void ping(int time);

private slots:
    void started();
    void finished(int exitCode, QProcess::ExitStatus exitStatus);
    void readyRead();

};

#endif // PING_H
