#ifndef EXTUPNP_H
#define EXTUPNP_H

#include "../measurement.h"
#include <QStringList>
#include <miniupnpc/miniupnpc.h>

class ExtUPnP : public Measurement
{
    Q_OBJECT
    Q_ENUMS(DataType)

public:
    explicit ExtUPnP(QObject *parent = 0);
    ~ExtUPnP();

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
        FriendlyName,
        ControlURL,
        RootDescURL
    };
    bool prepare(NetworkManager *networkManager, const MeasurementDefinitionPtr &measurementDefinition);
    bool start();
    bool stop();
    typedef QHash<DataType, QVariant> ExtUPnPHash;
    QList<ExtUPnPHash> goThroughDeviceList(UPNPDev * list);
    Result result() const;


private:
//    typedef QHash<DataType, QVariant> UPnPExtHash;
    QList<ExtUPnPHash> results;
};

#endif // EXTUPnP_H
