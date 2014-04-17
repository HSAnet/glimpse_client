#ifndef TRACEROUTE_H
#define TRACEROUTE_H

#include "../measurement.h"
#include "../udpping/udpping.h"
#include "traceroute_definition.h"

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

    TracerouteDefinitionPtr definition;
    Status currentStatus;
    UdpPing udpPing;

signals:
    void statusChanged(Status status);

public slots:
    void destinationUnreachable(PingProbe &probe);
    void ttlExceeded(PingProbe &probe);
};

#endif // TRACEROUTE_H
