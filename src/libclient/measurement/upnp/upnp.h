#ifndef UPNP_H
#define UPNP_H

#include "../upnpSniffer/UPnPHandler.h"

#include "../measurement.h"
#include "upnp_definition.h"
#include <miniupnpc/miniupnpc.h>

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
        EventSubUrl,
        ScpdURL,
        //ServiceType,
        URL,
        RootDescURL
    };
    typedef QHash<DataType, QVariant> UPnPHash;


    bool prepare(NetworkManager *networkManager, const MeasurementDefinitionPtr &measurementDefinition);
    bool start();
    bool stop();
    Result result() const;
    QList<UPnPHash> goThroughDeviceList(UPNPDev * list);

public slots:
    void waitUntilFinished();

private:
    QList<UPnPHash> results;

    UPnPDefinitionPtr definition;
    //Shall be searched for MediaServers? (from the definition)
    bool m_mediaServerSearch;
    UPnPHandler * m_handler;
};

#endif // UPNP_H
