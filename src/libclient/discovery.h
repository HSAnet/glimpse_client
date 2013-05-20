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
    Q_PROPERTY(bool running READ isRunning NOTIFY runningChanged)
    Q_PROPERTY(QString externalIpAddress READ externalIpAddress NOTIFY finished)
    Q_PROPERTY(QString lanIpAddress READ lanIpAddress NOTIFY finished)
    Q_PROPERTY(int linkLayerMaxUpload READ linkLayerMaxUpload NOTIFY finished)
    Q_PROPERTY(int linkLayerMaxDownload READ linkLayerMaxDownload NOTIFY finished)
    Q_PROPERTY(QString connectionType READ connectionType NOTIFY finished)
    Q_PROPERTY(QString natType READ natType NOTIFY finished)
    Q_PROPERTY(QString status READ status NOTIFY finished)
    Q_PROPERTY(int uptime READ uptime NOTIFY finished)
    Q_PROPERTY(QString lastConnectionError READ lastConnectionError NOTIFY finished)

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

    bool isRunning() const;

    /// @returns the discovered data
    DiscoveryHash data() const;

    QString externalIpAddress() const;
    QString lanIpAddress() const;
    int linkLayerMaxUpload() const;
    int linkLayerMaxDownload() const;
    QString connectionType() const;
    QString natType() const;
    QString status() const;
    int uptime() const;
    QString lastConnectionError() const;

public slots:
    void discover();

signals:
    void runningChanged();
    void started();
    void finished();

protected:
    class Private;
    Private* d;
    friend class Private;
};

#endif // DISCOVERY_H
