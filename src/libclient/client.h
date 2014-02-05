#ifndef CLIENT_H
#define CLIENT_H

#include "export.h"

#include <QObject>

class ConfigController;
class RegistrationController;
class LoginController;
class ReportController;
class TaskController;
class CrashController;
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
    Q_PROPERTY(Status status READ status NOTIFY statusChanged)
    Q_PROPERTY(Scheduler* scheduler READ scheduler CONSTANT)
    Q_PROPERTY(Settings* settings READ settings CONSTANT)
    Q_PROPERTY(ReportScheduler* reportScheduler READ reportScheduler CONSTANT)
    Q_PROPERTY(NetworkManager* networkManager READ networkManager CONSTANT)
    Q_PROPERTY(TaskExecutor* taskExecutor READ taskExecutor CONSTANT)
    Q_PROPERTY(ReportController* reportController READ reportController CONSTANT)
    Q_PROPERTY(ConfigController* configController READ configController CONSTANT)
    Q_PROPERTY(LoginController* loginController READ loginController CONSTANT)
    Q_PROPERTY(TaskController* taskController READ taskController CONSTANT)
    Q_PROPERTY(CrashController* crashController READ crashController CONSTANT)

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

    ConfigController* configController() const;
    LoginController* loginController() const;
    ReportController* reportController() const;
    TaskController* taskController() const;
    CrashController* crashController() const;

    Settings* settings() const;

public slots:
    bool init();
    bool autoLogin();
    void btc();
    void http();
    void upnp();
    void ping();
    void packetTrains();

signals:
    void statusChanged();

private:
    class Private;
    Private* d;
};

#endif // CLIENT_H
