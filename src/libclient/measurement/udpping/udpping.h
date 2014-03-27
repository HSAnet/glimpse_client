#ifndef UDPPING_H
#define UDPPING_H

#include <QtGlobal>
#include <QString>
#include <QVector>

#if defined(Q_OS_WIN)
#include <winsock2.h>
#include <Ws2ipdef.h>
#undef min
#elif defined(Q_OS_LINUX) || defined(Q_OS_OSX)
#include <netinet/in.h>
#else
#error Platform not supported.
#endif

#include "../measurement.h"
#include "udpping_definition.h"


class UdpPing : public Measurement
{
    Q_OBJECT

public:
    union sockaddr_any {
        struct sockaddr sa;
        struct sockaddr_in sin;
        struct sockaddr_in6 sin6;
    };

    struct pingProbe {
        int sock;
        quint64 sendTime;
        quint64 recvTime;
        sockaddr_any source;
    };

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
    bool sendData(struct pingProbe &probe);
    void receiveData(struct pingProbe &probe);
    void ping(struct pingProbe &probe);
    int getAddress(QString address, sockaddr_any *addr) const;

    UdpPingDefinitionPtr definition;
    Status currentStatus;
    QVector<struct pingProbe> m_pingProbes;

signals:
    void statusChanged(Status status);
    void destinationUnreachable(struct pingProbe &probe);
    void ttlExceeded(struct pingProbe &probe);
    void error(QString message);
};

#endif // UDPPING_H
