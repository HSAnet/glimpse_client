#ifndef HTTPDOWNLOAD_DEFINITION_H
#define HTTPDOWNLOAD_DEFINITION_H

#include "../measurementdefinition.h"

class HTTPDownloadDefinition;

typedef QSharedPointer<HTTPDownloadDefinition> HTTPDownloadDefinitionPtr;
typedef QList<HTTPDownloadDefinitionPtr> HTTPDownloadDefinitionList;

class HTTPDownloadDefinition : public MeasurementDefinition
{
public:
    HTTPDownloadDefinition(const QString &url, bool cacheTest);
    ~HTTPDownloadDefinition();

    // Storage
    static HTTPDownloadDefinitionPtr fromVariant(const QVariant &variant);

    // Getters
    QString url;
    bool cacheTest;

    // Serializable interface
    QVariant toVariant() const;
};

#endif // HTTPGETREQUEST_DEFINITION_H
