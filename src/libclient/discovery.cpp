#include "discovery.h"
#include "types.h"

#include <QStringList>
#include <QHostAddress>
#include <QFutureWatcher>
#include <QtConcurrentRun>

#ifdef HAVE_UPNP
#include <miniupnpc/miniupnpc.h>
#include <miniupnpc/upnpcommands.h>
#include <miniupnpc/miniwget.h>
#endif // HAVE_UPNP

#ifndef FALSE
#define FALSE 0
#define TRUE 1
#endif

class Discovery::Private : public QObject
{
    Q_OBJECT

public:
    Private(Discovery* q)
    : q(q)
    {
        connect(&watcher, SIGNAL(finished()), this, SLOT(discoveryFinished()));
    }

    Discovery* q;

    // Properties
    QFutureWatcher<Discovery::DiscoveryHash> watcher;
    Discovery::DiscoveryHash data;

    // Functions
    static Discovery::DiscoveryHash discover();
    static void upnpDiscover(Discovery::DiscoveryHash& hash);

public slots:
    void discoveryFinished();
};

Discovery::DiscoveryHash Discovery::Private::discover()
{
    Discovery::DiscoveryHash hash;
    upnpDiscover(hash);
    return hash;
}

QStringList GetValuesFromNameValueList(struct NameValueParserData * pdata,
                                       const char * Name)
{
    QStringList ret;
    struct NameValue * nv;

    for (nv = pdata->head.lh_first;
        (nv != NULL);
        nv = nv->entries.le_next)
    {
        if(strcmp(nv->name, Name) == 0)
            ret.append(nv->value);
    }

    return ret;
}

void Discovery::Private::upnpDiscover(Discovery::DiscoveryHash &hash)
{
#ifdef HAVE_UPNP
    int error = 0;

    UPNPDev* devlist = ::upnpDiscover(2000, NULL, NULL, FALSE, FALSE, &error);
    if (devlist) {
        UPNPUrls urls;
        IGDdatas data;
        char lanaddr[64];

        int code = UPNP_GetValidIGD(devlist, &urls, &data, lanaddr, sizeof(lanaddr));
        if (code == 1) {
            hash.insert(LanIpAddress, QLatin1String(lanaddr));

            char externalIP[40];
            if (UPNPCOMMAND_SUCCESS == UPNP_GetExternalIPAddress(urls.controlURL,
                                                                 data.first.servicetype,
                                                                 externalIP)) {
                hash.insert(ExternalIpAddress, QLatin1String(externalIP));
            }

            char connectionType[64];
            if(UPNPCOMMAND_SUCCESS == UPNP_GetConnectionTypeInfo(urls.controlURL,
                                                                 data.first.servicetype,
                                                                 connectionType)) {
                hash.insert(ConnectionType, QLatin1String(connectionType));
            }

            quint32 uplink, downlink;
            if (UPNPCOMMAND_SUCCESS == UPNP_GetLinkLayerMaxBitRates(urls.controlURL_CIF,
                                                                    data.CIF.servicetype,
                                                                    &downlink, &uplink)) {
                hash.insert(LinkLayerMaxDownload, downlink);
                hash.insert(LinkLayerMaxUpload, uplink);
            }

            char status[100];
            unsigned int uptime = 0;
            char lastConnectionError[128];
            if (UPNPCOMMAND_SUCCESS == UPNP_GetStatusInfo(urls.controlURL,
                                                          data.first.servicetype,
                                                          status,
                                                          &uptime,
                                                          lastConnectionError)) {
                hash.insert(Status, status);
                hash.insert(Uptime, uptime);
                hash.insert(LastConnectionError, lastConnectionError);
            }

            int bufferSize = 0;
            if (char* buffer = (char*)miniwget(urls.rootdescURL, &bufferSize, NULL)) {
                NameValueParserData pdata;
                ParseNameValue(buffer, bufferSize, &pdata);
                free(buffer); buffer = NULL;

                QStringList modelName = GetValuesFromNameValueList(&pdata, "modelName");
                if ( !modelName.isEmpty() )
                    hash.insert(ModelName, modelName.last());

                QStringList manufacturer = GetValuesFromNameValueList(&pdata, "manufacturer");
                if ( !manufacturer.isEmpty() )
                    hash.insert(Manufacturer, manufacturer.last());

                QStringList friendlyName = GetValuesFromNameValueList(&pdata, "friendlyName");
                if ( !friendlyName.isEmpty() )
                    hash.insert(FriendlyName, friendlyName.last());
            }
        }

        FreeUPNPUrls(&urls);
        freeUPNPDevlist(devlist);
    }
#else
    Q_UNUSED(hash)
#endif // HAVE_UPNP
}

void Discovery::Private::discoveryFinished()
{
    data = watcher.result();
    emit q->finished();
}

Discovery::Discovery(QObject *parent)
: QObject(parent)
, d(new Private(this))
{
    connect(&d->watcher, SIGNAL(started()), this, SIGNAL(runningChanged()));
    connect(&d->watcher, SIGNAL(finished()), this, SIGNAL(runningChanged()));
}

Discovery::~Discovery()
{
    delete d;
}

bool Discovery::isRunning() const
{
    return d->watcher.isRunning();
}

bool Discovery::hasData() const
{
    return !d->data.isEmpty();
}

Discovery::DiscoveryHash Discovery::data() const
{
    return d->data;
}

QString Discovery::externalIpAddress() const
{
    return d->data.value(ExternalIpAddress).toString();
}

QString Discovery::lanIpAddress() const
{
    return d->data.value(LanIpAddress).toString();
}

int Discovery::linkLayerMaxUpload() const
{
    return d->data.value(LinkLayerMaxUpload).toInt();
}

int Discovery::linkLayerMaxDownload() const
{
    return d->data.value(LinkLayerMaxDownload).toInt();
}

QString Discovery::connectionType() const
{
    return d->data.value(ConnectionType).toString();
}

QString Discovery::natType() const
{
    return d->data.value(NatType).toString();
}

QString Discovery::status() const
{
    return d->data.value(Status).toString();
}

int Discovery::uptime() const
{
    return d->data.value(Uptime).toInt();
}

QString Discovery::lastConnectionError() const
{
    return d->data.value(LastConnectionError).toString();
}

QString Discovery::modelName() const
{
    return d->data.value(ModelName).toString();
}

QString Discovery::manufacturer() const
{
    return d->data.value(Manufacturer).toString();
}

QString Discovery::friendlyName() const
{
    return d->data.value(FriendlyName).toString();
}

QVariant Discovery::result() const
{
    // Prepare upnp data
    QVariantMap upnp;
    QHashIterator<Discovery::DataType, QVariant> iter(d->data);
    while ( iter.hasNext() ) {
        iter.next();

        QString name = enumToString(Discovery, "DataType", iter.key());
        name = name.replace(QRegExp("([A-Z])"), "-\\1").toLower();
        name.remove(0, 1);

        upnp.insert(name, iter.value());
    }

    return upnp;
}

void Discovery::discover()
{
    d->data.clear();
    emit started();
    d->watcher.setFuture( QtConcurrent::run(Discovery::Private::discover) );
}

#include "discovery.moc"
