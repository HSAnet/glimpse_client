#ifndef HTTPGETREQUEST_H
#define HTTPGETREQUEST_H

#include "../measurement.h"
#include "httpdownload_definition.h"

#include <QElapsedTimer>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QList>

class HTTPDownload : public Measurement
{
    Q_OBJECT

public:
    explicit HTTPDownload(QObject *parent = 0);
    ~HTTPDownload();

    // Measurement interface
    Status status() const;
    bool prepare(NetworkManager *networkManager, const MeasurementDefinitionPtr &measurementDefinition);
    bool start();
    bool stop();
    Result result() const;

private:
    void setStatus(Status status);
    HTTPDownloadDefinitionPtr definition;
    QElapsedTimer m_timer;
    qint64 m_lasttime;
    QVector<qint64> m_bytesReceived;
    QVector<qint64> m_times;
    QList<qreal> m_downloadSpeeds;
    Status currentStatus;

    QNetworkAccessManager *m_nam;
    QNetworkRequest m_request;
    QNetworkReply *m_reply;

private slots:
    void requestFinished();
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);

signals:
    void statusChanged(Status status);
};

#endif // HTTPGETREQUEST_H
