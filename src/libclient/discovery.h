#ifndef DISCOVERY_H
#define DISCOVERY_H

#include <QObject>
#include <QHash>

/**
 * The Discovery class
 *
 * Discovers information about the local network such as
 * DSL connectivity speed, public ip address ...
 */
class Discovery : public QObject
{
    Q_OBJECT
    Q_ENUMS(DataType)

public:
    explicit Discovery(QObject *parent = 0);
    ~Discovery();

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
        LastConnectionError
    };

    typedef QHash<DataType, QVariant> DiscoveryHash;

    /// @returns the discovered data
    DiscoveryHash data() const;

public slots:
    void discover();

signals:
    void finished();

protected:
    class Private;
    Private* d;
    friend class Private;
};

#endif // DISCOVERY_H
