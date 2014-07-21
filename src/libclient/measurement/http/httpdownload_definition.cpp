#include "httpdownload_definition.h"

HTTPDownloadDefinition::HTTPDownloadDefinition(const QString &url, const bool cacheTest, const int threads)
: url(url)
, cacheTest(cacheTest)
, threads(threads)
{

}

HTTPDownloadDefinition::~HTTPDownloadDefinition()
{

}

HTTPDownloadDefinitionPtr HTTPDownloadDefinition::fromVariant(const QVariant &variant)
{
    QVariantMap map = variant.toMap();
    return HTTPDownloadDefinitionPtr(new HTTPDownloadDefinition(map.value("url", "").toString(),
                                                                map.value("cache_test", false).toBool(),
                                                                map.value("threads", 1).toInt()));
}

QVariant HTTPDownloadDefinition::toVariant() const
{
    QVariantMap map;
    map.insert("url", url);
    map.insert("cache_test", cacheTest);
    map.insert("threads", threads);
    return map;
}
