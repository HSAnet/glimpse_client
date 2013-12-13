#ifndef HTTPGETREQUEST_H
#define HTTPGETREQUEST_H

#include "../measurement.h"
#include "httpgetrequest_definition.h"

#include <QNetworkReply>
#include <QNetworkRequest>

class HTTPGetRequest : public Measurement
{
    Q_OBJECT

public:
    explicit HTTPGetRequest(QObject *parent = 0);
    ~HTTPGetRequest();

    // Measurement interface
    Status status() const;
    bool prepare(NetworkManager* networkManager, const MeasurementDefinitionPtr& measurementDefinition);
    bool start();
    bool stop();
    ResultPtr result() const;

private:
    void setStatus(Status status);
    HTTPGetRequestDefinitionPtr definition;
    Status currentStatus;

    QNetworkAccessManager* nam;
    QNetworkRequest request;
    QNetworkReply* reply;

private slots:
    void started();
    void finished(QNetworkReply* reply);

signals:
    void statusChanged(Status status);
};

#endif // HTTPGETREQUEST_H
