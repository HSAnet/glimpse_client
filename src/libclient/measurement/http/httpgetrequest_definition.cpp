#include "httpgetrequest_definition.h"

HTTPGetRequestDefinition::HTTPGetRequestDefinition(const QString &url, bool cacheTest)
    : url(url)
    , cacheTest(cacheTest)
{

}

HTTPGetRequestDefinition::~HTTPGetRequestDefinition()
{

}

HTTPGetRequestDefinitionPtr HTTPGetRequestDefinition::fromVariant(const QVariant &variant)
{
    QVariantMap map = variant.toMap();
    return HTTPGetRequestDefinitionPtr(new HTTPGetRequestDefinition(map.value("url").toString(),
                                                                    map.value("cacheTest").toBool()));
}

QVariant HTTPGetRequestDefinition::toVariant() const
{
    QVariantMap map;
    map.insert("url", url);
    map.insert("cacheTest", cacheTest);
    return map;
}
