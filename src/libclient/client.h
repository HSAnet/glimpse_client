#ifndef CLIENT_H
#define CLIENT_H

#include "testfactory.h"
#include "testscheduler.h"
#include "types.h"

#include <QObject>
#include <QLocale>
#include <QHostAddress>
#include <QStringList>

class QNetworkAccessManager;

////////////////////////////////////////////////////////////

class Client : public QObject
{
    Q_OBJECT
    Q_ENUMS(Status)
    Q_PROPERTY(int status READ status NOTIFY statusChanged)
    Q_PROPERTY(TestScheduler* scheduler READ scheduler CONSTANT)

    explicit Client(QObject *parent = 0);
    ~Client();

public:
    /// Receives the main client instance
    static Client* instance();

    enum Status
    {
        Unregistered,
        Registered
    };

    /// Get the current status
    Status status() const;

    QNetworkAccessManager* networkAccessManager() const;

    QAbstractSocket* managerSocket() const;

    TestScheduler* scheduler() const;

public slots:
    bool init();
    void requestTest();

signals:
    void statusChanged();

private:
    class Private;
    Private* d;
};

#endif // CLIENT_H
