#ifndef EXTENDEDUPNP_H
#define EXTENDEDUPNP_H


#include "../upnpSniffer/UPnPHandler.h"

#include "../measurement.h"
#include <miniupnpc/miniupnpc.h>
#include <QObject>

class extendedUPnP : public Measurement
{
    Q_OBJECT
    Q_ENUMS(DataType)

public:
    extendedUPnP(QObject *parent = 0);
    ~extendedUPnP();
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
        ControlUrl,
        EventSubUrl,
        ScpdUrl,
        ServiceType,
        FoundContent,
        URL,
        RootDescUrl,
        UDN
    };
    typedef QHash<DataType, QVariant> UPnPHash;

    bool prepare(NetworkManager *networkManager, const MeasurementDefinitionPtr &measurementDefinition);
    bool start();
    bool stop();
    Result result() const;
    QList<UPnPHash> quickDevicesCheck(UPNPDev * list);
    void printResultsToMap(QVariantList *list);

signals:
    void done();

public slots:
    void waitUntilFinished();

private:
    QList<UPnPHash> results;
    QVariantList additional_res;

    //UPnPDefinitionPtr definition;
    bool m_mediaServerSearch;
    UPnPHandler * m_handler;
};

#endif // EXTENDEDUPNP_H
