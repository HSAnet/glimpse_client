#ifndef UPNP_H
#define UPNP_H

#include "../measurement.h"
#include <QStringList>

class UPnP : public Measurement
{
    Q_OBJECT
    Q_ENUMS(DataType)

public:
    explicit UPnP(QObject *parent = 0);
    ~UPnP();

    // Measurement interface
    Status status() const;
    bool prepare(NetworkManager* networkManager, const MeasurementDefinitionPtr& measurementDefinition);
    bool start();
    bool stop();
    ResultPtr result() const;

    enum DataType
    {
        ExternalIpAddress,
        LanIpAddress,
        LinkLayerMaxUpload,
        LinkLayerMaxDownload,
        TotalBytesSent,
        TotalBytesReceived,
        TotalPacketsSent,
        TotalPacketsReceived,
        ConnectionType,
        NatType,
        Status,
        Uptime,
        LastConnectionError,
        NumberOfPortMappings,
        FirewallEnabled,
        InboundPinholeAllowed,

        ModelName,
        Manufacturer,
        FriendlyName
    };

private:
    typedef QHash<DataType, QVariant> UPnPHash;
    QList<UPnPHash> results;
};

#endif // UPNP_H
