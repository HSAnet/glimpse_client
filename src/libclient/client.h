#ifndef CLIENT_H
#define CLIENT_H

#include "export.h"

#include <QObject>

class Scheduler;
class ReportScheduler;
class NetworkManager;
class TaskExecutor;
class Settings;
class QNetworkAccessManager;

////////////////////////////////////////////////////////////

class CLIENT_API Client : public QObject
{
    Q_OBJECT
    Q_ENUMS(Status)
    Q_PROPERTY(Status status READ status WRITE setStatus NOTIFY statusChanged)
    Q_PROPERTY(Scheduler* scheduler READ scheduler CONSTANT)
    Q_PROPERTY(Settings* settings READ settings CONSTANT)
    Q_PROPERTY(ReportScheduler* reportScheduler READ reportScheduler CONSTANT)
    Q_PROPERTY(NetworkManager* networkManager READ networkManager CONSTANT)
    Q_PROPERTY(TaskExecutor* taskExecutor READ taskExecutor CONSTANT)

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
    void setStatus(Status status);
    Status status() const;

    QNetworkAccessManager* networkAccessManager() const;

    Scheduler* scheduler() const;
    ReportScheduler* reportScheduler() const;
    NetworkManager* networkManager() const;
    TaskExecutor* taskExecutor() const;

    Settings* settings() const;

public slots:
    bool init();

signals:
    void statusChanged();

private:
    class Private;
    Private* d;
};

#endif // CLIENT_H
