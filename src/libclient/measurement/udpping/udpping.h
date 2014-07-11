#ifndef UDPPING_H
#define UDPPING_H

#include <QtGlobal>
#include <QString>
#include <QVector>

#if defined(Q_OS_WIN)
#include <QtConcurrent/QtConcurrentRun>
#include <QMutex>
#include <winsock2.h>
#include <Ws2ipdef.h>
#include <WS2tcpip.h>
#undef min
#include<pcap.h>
#include <cstring>
#elif defined(Q_OS_LINUX) || defined(Q_OS_OSX)
#include <netinet/in.h>
#else
#error Platform not supported.
#endif

#include "../measurement.h"
#include "udpping_definition.h"

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
namespace udpping
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
    udpping::PacketType type;
    udpping::Response response;
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
#if defined(Q_OS_OSX)
    int icmpSock;
#endif
};

class UdpPing : public Measurement
{
    Q_OBJECT

public:

    explicit UdpPing(QObject *parent = 0);
    ~UdpPing();

    // Measurement interface
    Status status() const;
    bool prepare(NetworkManager *networkManager, const MeasurementDefinitionPtr &measurementDefinition);
    bool start();
    bool stop();
    Result result() const;

private:
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

    UdpPingDefinitionPtr definition;
    Status currentStatus;
    QVector<PingProbe> m_pingProbes;

    pcap_if_t *m_device;
    pcap_t *m_capture;
    sockaddr_any m_destAddress;


signals:
    void statusChanged(Status status);
    void destinationUnreachable(const PingProbe &probe);
    void ttlExceeded(const PingProbe &probe);
    void udpResponse(const PingProbe &probe);
    void timeout(const PingProbe &probe);
    void tcpReset(const PingProbe &probe);
    void tcpConnect(const PingProbe &probe);
};

#endif // UDPPING_H
