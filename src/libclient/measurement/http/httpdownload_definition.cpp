#include "httpdownload_definition.h"

HTTPDownloadDefinition::HTTPDownloadDefinition(const QString &url, const bool cacheTest, const int threads, \
                                               const int targetTime, const int rampUpTime, const int slotLength)
: url(url)
, avoidCaches(cacheTest)
, threads(threads)
, targetTime(targetTime)
, rampUpTime(rampUpTime)
, slotLength(slotLength)

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
                                                                map.value("slot_length", 1000).toInt()));
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
    return map;
}
