#ifndef UPNP_H
#define UPNP_H

#include "../measurement.h"
#include <QStringList>
#include <miniupnpc/miniupnpc.h>

class UPnP : public Measurement
{
    Q_OBJECT
    Q_ENUMS(DataType)

public:
    explicit UPnP(QObject *parent = 0);
    ~UPnP();

    // Measurement interface
    Status status() const;
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
    bool prepare(NetworkManager *networkManager, const MeasurementDefinitionPtr &measurementDefinition);
    bool start();
    bool stop();
    typedef QHash<DataType, QVariant> UPnPHash;
    QList<UPnPHash> goThroughDeviceList(UPNPDev * list);
    Result result() const;


private:
//    typedef QHash<DataType, QVariant> UPnPExtHash;
    QList<UPnPHash> results;
};

#endif // UPnPExt_H
