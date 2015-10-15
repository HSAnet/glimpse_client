#include "httpdownload_definition.h"

HTTPDownloadDefinition::HTTPDownloadDefinition(const QString &url, const bool cacheTest, const int threads, \
                                               const int targetTime, const int rampUpTime, const int slotLength,
                                               const quint16 sourcePort)
: url(url)
, avoidCaches(cacheTest)
, threads(threads)
, targetTime(targetTime)
, rampUpTime(rampUpTime)
, slotLength(slotLength)
, sourcePort(sourcePort)
{

}

HTTPDownloadDefinition::~HTTPDownloadDefinition()
{

}

HTTPDownloadDefinitionPtr HTTPDownloadDefinition::fromVariant(const QVariant &variant)
{
    QVariantMap map = variant.toMap();
    return HTTPDownloadDefinitionPtr(new HTTPDownloadDefinition(map.value("url", "").toString(),
                                                                map.value("avoid_caches", false).toBool(),
                                                                map.value("threads", 1).toInt(),
                                                                map.value("target_time", 10000).toInt(),
                                                                map.value("ramp_up_time", 3000).toInt(),
                                                                map.value("slot_length", 1000).toInt(),
                                                                map.value("source_port", 0).toUInt()));
}

HTTPDownloadDefinitionPtr HTTPDownloadDefinition::fromSpecification(const QVariant &variant)
{
    QVariantMap map = variant.toMap();
    if (map.isEmpty())
    {
        return HTTPDownloadDefinitionPtr();
    }

    return HTTPDownloadDefinitionPtr(new HTTPDownloadDefinition(map.value("destination.url", "").toString(),
                                                                map.value("glimpse.http.avoid-caches", false).toBool(),
                                                                map.value("glimpse.http.thread.count", 1).toInt(),
                                                                map.value("glimpse.http.target-time", 10000).toInt(),
                                                                map.value("glimpse.http.ramp-up-time", 3000).toInt(),
                                                                map.value("glimpse.http.slot-length", 1000).toInt(),
                                                                map.value("glimpse.http.source-port", 0).toInt()));
}

QVariant HTTPDownloadDefinition::toVariant() const
{
    QVariantMap map;
    map.insert("url", url);
    map.insert("avoid_caches", avoidCaches);
    map.insert("threads", threads);
    map.insert("target_time", targetTime);
    map.insert("ramp_up_time", rampUpTime);
    map.insert("slot_length", slotLength);
    map.insert("source_port", sourcePort);
    return map;
}

QVariantMap HTTPDownloadDefinition::capability()
{
    QVariantMap parameters;
    parameters.insert("destination.url", "*");
    //parameters.insert("glimpse.http.avoid-caches", "*"); // TODO registry
    parameters.insert("glimpse.http.thread.count", "*");
    //parameters.insert("glimpse.http.target-time", "*"); // TODO
    //parameters.insert("glimpse.http.ramp-up-time", "*"); // TODO
    //parameters.insert("glimpse.http.slot-length", "*"); // TODO
    //parameters.insert("glimpse.http.source-port", "*"); // TODO

    QVariantList results;
    results.append("glimpse.http.bandwidth.imputed.bps.avg");
    results.append("glimpse.http.thread.count");
    results.append("untainted");
    //results.append("bandwidth_bps_per_thread");

    QVariantMap capability;
    capability.insert("capability", "measure");
    capability.insert("label", "glimpse-httpdownload");
    capability.insert("parameters", parameters);
    capability.insert("results", results);
    capability.insert("version", 0);
    capability.insert("when", "now");
    capability.insert("registry", "https://www.measure-it.net/static/glimpse_registry.json");

    return capability;
}

