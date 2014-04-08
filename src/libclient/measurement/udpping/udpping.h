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
enum Response
{
    DESTINATION_UNREACHABLE,
    TTL_EXCEEDED,
    UNHANDLED_ICMP
};
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
    Response response;
    quint8 icmpType;
    quint8 icmpCode;
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
    bool prepare(NetworkManager* networkManager, const MeasurementDefinitionPtr& measurementDefinition);
    bool start();
    bool stop();
    ResultPtr result() const;

private:
    void setStatus(Status status);
    int initSocket();
    bool sendData(PingProbe *probe);
    void receiveData(PingProbe *probe);
    void ping(PingProbe *probe);

    UdpPingDefinitionPtr definition;
    Status currentStatus;
    QVector<PingProbe> m_pingProbes;

    pcap_if_t *m_device;
    pcap_t *m_capture;

signals:
    void statusChanged(Status status);
    void destinationUnreachable(PingProbe &probe);
    void ttlExceeded(PingProbe &probe);
    void error(QString message);
};

#endif // UDPPING_H
