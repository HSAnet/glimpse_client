#include "httpdownload_plugin.h"
#include "httpdownload.h"
#include "httpdownload_definition.h"

QStringList HTTPDownloadPlugin::measurements() const
{
    return QStringList()
           << "httpdownload";
}

MeasurementPtr HTTPDownloadPlugin::createMeasurement(const QString &name)
{
    Q_UNUSED(name);
    return MeasurementPtr(new HTTPDownload);
}

MeasurementDefinitionPtr HTTPDownloadPlugin::createMeasurementDefinition(const QString &name, const QVariant &data)
{
    Q_UNUSED(name);
    return HTTPDownloadDefinition::fromVariant(data);
}
