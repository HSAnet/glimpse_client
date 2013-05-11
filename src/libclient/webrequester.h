#ifndef WEBREQUESTER_H
#define WEBREQUESTER_H

#include "requests.h"

#include <QObject>
#include <QJsonObject>

class WebRequester : public QObject
{
    Q_OBJECT
    Q_ENUMS(Status)
    Q_PROPERTY(int status READ status NOTIFY statusChanged)
    Q_PROPERTY(Request* request READ request WRITE setRequest NOTIFY requestChanged)
    Q_PROPERTY(bool running READ isRunning NOTIFY statusChanged)

public:
    explicit WebRequester(QObject *parent = 0);
    ~WebRequester();

    enum Status {
        Unknown,
        Running,
        Finished,
        Error
    };

    Status status() const;

    void setRequest(Request* request);
    Request* request() const;

    bool isRunning() const;

    Q_INVOKABLE QString errorString() const;

    Q_INVOKABLE QVariant jsonDataQml() const;
    QJsonObject jsonData() const;

public slots:
    void start();

signals:
    void statusChanged(Status status);
    void requestChanged(Request* request);

protected:
    class Private;
    friend class Private;
    Private* d;
};

#endif // WEBREQUESTER_H
