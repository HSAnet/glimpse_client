#ifndef TRACEROUTE_H
#define TRACEROUTE_H

#include <QtGlobal>
#include <QMutex>
#include <QVector>

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
    TIMEOUT
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
    bool prepare(NetworkManager* networkManager,
                 const MeasurementDefinitionPtr& measurementDefinition);
    bool start();
    bool stop();
    ResultPtr result() const;

private:
    void setStatus(Status status);
    void ping();

    TracerouteDefinitionPtr definition;
    Status currentStatus;
    UdpPing udpPing;
    QVector<Hop> hops;
    bool receivedDestinationUnreachable;
    int ttl;

signals:
    void statusChanged(Status status);
    void handledResponse();

public slots:
    void destinationUnreachable(PingProbe &probe);
    void ttlExceeded(PingProbe &probe);
    void timeout(PingProbe &probe);
    void pingFinished();
};

#endif // TRACEROUTE_H
