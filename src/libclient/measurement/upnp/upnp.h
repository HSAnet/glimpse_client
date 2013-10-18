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
        ConnectionType,
        NatType,
        Status,
        Uptime,
        LastConnectionError,

        ModelName,
        Manufacturer,
        FriendlyName
    };

private:

    typedef QHash<DataType, QVariant> UPnPHash;
    UPnPHash resultHash;

private slots:

};

#endif // UPNP_H
