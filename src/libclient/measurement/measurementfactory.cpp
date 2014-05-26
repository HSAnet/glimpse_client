#include "measurementfactory.h"
#include "btc/btc_plugin.h"
#include "http/httpdownload_plugin.h"
#include "upnp/upnp_plugin.h"
#include "ping/ping_plugin.h"
#include "dnslookup/dnslookup_plugin.h"
#include "reverse_dnslookup/reverseDnslookup_plugin.h"
#include "packettrains/packettrainsplugin.h"
#include "udpping/udpping_plugin.h"
#include "traceroute/traceroute_plugin.h"
#include "../log/logger.h"

#include <QHash>

LOGGER(MeasurementFactory);

class MeasurementFactory::Private
{
public:
    Private()
    {
        // TODO: Don't link with plugins
        addPlugin(new BulkTransportCapacityPlugin);
        addPlugin(new HTTPDownloadPlugin);
        addPlugin(new UPnPPlugin);
        addPlugin(new PingPlugin);
        addPlugin(new DnslookupPlugin);
        addPlugin(new ReverseDnslookupPlugin);
        addPlugin(new PacketTrainsPlugin);
        addPlugin(new UdpPingPlugin);
        addPlugin(new TraceroutePlugin);
    }

    ~Private()
    {
        qDeleteAll(plugins);
    }

    // Properties
    QList<MeasurementPlugin*> plugins;
    QHash<QString, MeasurementPlugin*> pluginNameHash;

    // Functions
    void addPlugin(MeasurementPlugin* plugin);
};

void MeasurementFactory::Private::addPlugin(MeasurementPlugin *plugin)
{
    plugins.append(plugin);

    foreach(const QString& name, plugin->measurements())
    {
        pluginNameHash.insert(name, plugin);
    }
}

MeasurementFactory::MeasurementFactory()
: d(new Private)
{
}

MeasurementFactory::~MeasurementFactory()
{
    delete d;
}

MeasurementPluginList MeasurementFactory::plugins() const
{
    return d->plugins;
}

QStringList MeasurementFactory::availableMeasurements() const
{
    return d->pluginNameHash.keys();
}

MeasurementPtr MeasurementFactory::createMeasurement(const QString &name, const QUuid &id)
{
    if (MeasurementPlugin* plugin = d->pluginNameHash.value(name))
    {
        MeasurementPtr ptr = plugin->createMeasurement(name);
        ptr->setTaskId(id);
        return ptr;
    }

    return MeasurementPtr();
}

MeasurementDefinitionPtr MeasurementFactory::createMeasurementDefinition(const QString &name, const QVariant &data)
{
    if (MeasurementPlugin* plugin = d->pluginNameHash.value(name))
    {
        return plugin->createMeasurementDefinition(name, data);
    }
    else
    {
        LOG_DEBUG(QString("No measurement named '%1'' found.").arg(name));
    }

    return MeasurementDefinitionPtr();
}
