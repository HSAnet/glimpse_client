#ifndef WEBREQUESTER_H
#define WEBREQUESTER_H

#include "network/requests/request.h"
#include "network/responses/response.h"

#include <QObject>
#include <QJsonObject>
#include <QUrl>
#include <QSslError>

class CLIENT_API WebRequester : public QObject
{
    Q_OBJECT
    Q_ENUMS(Status)
    Q_PROPERTY(Status status READ status NOTIFY statusChanged)
    Q_PROPERTY(int timeout READ timeout WRITE setTimeout NOTIFY timeoutChanged)
    Q_PROPERTY(QUrl url READ url WRITE setUrl NOTIFY urlChanged)
    Q_PROPERTY(Request *request READ request WRITE setRequest NOTIFY requestChanged)
    Q_PROPERTY(Response *response READ response WRITE setResponse NOTIFY responseChanged)
    Q_PROPERTY(bool running READ isRunning NOTIFY statusChanged)

public:
    explicit WebRequester(QObject *parent = 0);
    ~WebRequester();

    enum Status
    {
        Unknown,
        Running,
        Finished,
        Error
    };

    Status status() const;

    void setTimeout(int ms);
    int timeout() const;

    void setUrl(const QUrl &url);
    QUrl url() const;

    void setRequest(Request *request);
    Request *request() const;

    void setResponse(Response *response);
    Response *response() const;

    bool isRunning() const;

    Q_INVOKABLE QString errorString() const;

    Q_INVOKABLE QVariant jsonDataQml() const;
    QJsonObject jsonData() const;

public slots:
    void start();
    void sslErrors(QList<QSslError> errors);

signals:
    void statusChanged(WebRequester::Status status);
    void timeoutChanged(int timeout);
    void urlChanged(const QUrl &url);
    void requestChanged(Request *request);
    void responseChanged(Response *response);

    void started();
    void finished();
    void error();

protected:
    class Private;
    friend class Private;
    Private *d;
};

#endif // WEBREQUESTER_H
