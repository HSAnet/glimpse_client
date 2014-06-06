#ifndef REQUEST_H
#define REQUEST_H

#include "export.h"

#include <QObject>
#include <QUuid>
#include <QVariant>

class CLIENT_API Request : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString deviceId READ deviceId WRITE setDeviceId NOTIFY deviceIdChanged)
    Q_PROPERTY(QString sessionId READ sessionId WRITE setSessionId NOTIFY sessionIdChanged)

public:
    explicit Request(QObject *parent = 0);
    ~Request();

    virtual QVariant toVariant() const = 0;

    void setDeviceId(const QString &deviceId);
    QString deviceId() const;

    void setSessionId(const QString &sessionId);
    QString sessionId() const;

signals:
    void deviceIdChanged(const QUuid &deviceId);
    void sessionIdChanged(const QString &sessionId);

protected:
    class Private;
    friend class Private;
    Private *d;
};

#endif // REQUEST_H
