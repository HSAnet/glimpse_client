#ifndef HTTPDOWNLOAD_DEFINITION_H
#define HTTPDOWNLOAD_DEFINITION_H

#include "../measurementdefinition.h"

class HTTPDownloadDefinition;

typedef QSharedPointer<HTTPDownloadDefinition> HTTPDownloadDefinitionPtr;
typedef QList<HTTPDownloadDefinitionPtr> HTTPDownloadDefinitionList;

class HTTPDownloadDefinition : public MeasurementDefinition
{
public:
    HTTPDownloadDefinition(const QString &url, const bool avoidCaches, const int threads,
                           const int targetTime, const int rampUpTime, const int slotLength);
    ~HTTPDownloadDefinition();

    // Storage
    static HTTPDownloadDefinitionPtr fromVariant(const QVariant &variant);
    static HTTPDownloadDefinitionPtr fromSpecification(const QVariant &variant);
    static QVariantMap capability();

    // Getters
    QString url;
    bool avoidCaches;
    int threads;
    int targetTime;
    int rampUpTime;
    int slotLength;

    // Serializable interface
    QVariant toVariant() const;
};

#endif // HTTPGETREQUEST_DEFINITION_H
