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

enum Response
{
    TTL_EXCEEDED,
    DESTINATION_UNREACHABLE,
    TIMEOUT
};

struct Hop
{
    PingProbe probe;
    Response response;
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
    void traceroute();
    void ping(quint32 ttl);

    TracerouteDefinitionPtr definition;
    Status currentStatus;
    UdpPing udpPing;
    QVector<Hop> hops;
    QMutex mutex;

signals:
    void statusChanged(Status status);
    void handledResponse();

public slots:
    void destinationUnreachable(PingProbe &probe);
    void ttlExceeded(PingProbe &probe);
    void timeout(PingProbe &probe);
};

#endif // TRACEROUTE_H
