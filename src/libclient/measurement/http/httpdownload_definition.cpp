#include "httpdownload_definition.h"

HTTPDownloadDefinition::HTTPDownloadDefinition(const QString &url, bool cacheTest)
    : url(url)
    , cacheTest(cacheTest)
{

}

HTTPDownloadDefinition::~HTTPDownloadDefinition()
{

}

HTTPDownloadDefinitionPtr HTTPDownloadDefinition::fromVariant(const QVariant &variant)
{
    QVariantMap map = variant.toMap();
    return HTTPDownloadDefinitionPtr(new HTTPDownloadDefinition(map.value("url", "").toString(),
                                                                map.value("cacheTest", false).toBool()));
}

QVariant HTTPDownloadDefinition::toVariant() const
{
    QVariantMap map;
    map.insert("url", url);
    map.insert("cacheTest", cacheTest);
    return map;
}
