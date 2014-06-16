#ifndef TRACEROUTE_H
#define TRACEROUTE_H

#include <QtGlobal>
#include <QMutex>
#include <QList>

#include "../measurement.h"
#include "../../task/task.h"
#include "../udpping/udpping.h"
#include "../udpping/udpping_plugin.h"
#include "../udpping/udpping_definition.h"
#include "traceroute_definition.h"

namespace traceroute
{
    enum Response
    {
        TTL_EXCEEDED,
        DESTINATION_UNREACHABLE,
        TIMEOUT,
        UDP_RESPONSE
    };
}

struct Hop
{
    PingProbe probe;
    traceroute::Response response;
};

class Traceroute : public Measurement
{
    Q_OBJECT
public:
    explicit Traceroute(QObject *parent = 0);
    ~Traceroute();

    // Measurement interface
    Status status() const;
    bool prepare(NetworkManager *networkManager,
                 const MeasurementDefinitionPtr &measurementDefinition);
    bool start();
    bool stop();
    ResultPtr result() const;

private:
    void setStatus(Status status);
    void ping();

    TracerouteDefinitionPtr definition;
    Status currentStatus;
    UdpPing udpPing;
    QList<Hop> hops;
    bool endOfRoute;
    int ttl;

signals:
    void statusChanged(Status status);
    void handledResponse();

public slots:
    void destinationUnreachable(const PingProbe &probe);
    void ttlExceeded(const PingProbe &probe);
    void timeout(const PingProbe &probe);
    void udpResponse(const PingProbe &probe);
    void pingFinished();
};

#endif // TRACEROUTE_H
