#ifndef HTTPGETREQUEST_DEFINITION_H
#define HTTPGETREQUEST_DEFINITION_H

#include "../measurementdefinition.h"

class HTTPGetRequestDefinition;

typedef QSharedPointer<HTTPGetRequestDefinition> HTTPGetRequestDefinitionPtr;
typedef QList<HTTPGetRequestDefinitionPtr> HTTPGetRequestDefinitionList;

class HTTPGetRequestDefinition : public MeasurementDefinition
{
public:
    HTTPGetRequestDefinition(const QString& url, bool cacheTest);
    ~HTTPGetRequestDefinition();

    // Storage
    static HTTPGetRequestDefinitionPtr fromVariant(const QVariant &variant);

    // Getters
    QString url;
    bool cacheTest;

    // Serializable interface
    QVariant toVariant() const;
};

#endif // HTTPGETREQUEST_DEFINITION_H
