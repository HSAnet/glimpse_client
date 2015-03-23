#ifndef CLIENT_H
#define CLIENT_H

#include "export.h"
#include "types.h"

#include <QObject>
#include <QAbstractSocket>

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
class TrafficBudgetManager;

////////////////////////////////////////////////////////////

class CLIENT_API Client : public QObject
{
    Q_OBJECT
    Q_ENUMS(Status ProbeType)
    Q_PROPERTY(Status status READ status NOTIFY statusChanged)
    Q_PROPERTY(Scheduler *scheduler READ scheduler CONSTANT)
    Q_PROPERTY(Settings *settings READ settings CONSTANT)
    Q_PROPERTY(ReportScheduler *reportScheduler READ reportScheduler CONSTANT)
    Q_PROPERTY(NetworkManager *networkManager READ networkManager CONSTANT)
    Q_PROPERTY(TaskExecutor *taskExecutor READ taskExecutor CONSTANT)
    Q_PROPERTY(ReportController *reportController READ reportController CONSTANT)
    Q_PROPERTY(ConfigController *configController READ configController CONSTANT)
    Q_PROPERTY(LoginController *loginController READ loginController CONSTANT)
    Q_PROPERTY(TaskController *taskController READ taskController CONSTANT)
    Q_PROPERTY(CrashController *crashController READ crashController CONSTANT)

    explicit Client(QObject *parent = 0);
    ~Client();

public:
    /// Receives the main client instance
    static Client *instance();

    enum Status
    {
        Unregistered,
        Registered
    };

    /// Get the current status
    void setStatus(Status status);
    Status status() const;

    // Get the version
    static QString version();

    QNetworkAccessManager *networkAccessManager() const;

    Scheduler *scheduler() const;
    ReportScheduler *reportScheduler() const;
    NetworkManager *networkManager() const;
    TaskExecutor *taskExecutor() const;

    ConfigController *configController() const;
    LoginController *loginController() const;
    ReportController *reportController() const;
    TaskController *taskController() const;
    CrashController *crashController() const;

    Settings *settings() const;
    TrafficBudgetManager *trafficBudgetManager() const;

    /* Versioning
     *
     * MAJOR version when you make incompatible API changes,
     * MINOR version when you add API-functionality in a backwards-compatible manner, and
     * PATCH version when you make backwards-compatible bug fixes.
     *
     * Source: http://semver.org
     */
    static const quint32 versionMajor = 2;
    static const quint32 versionMinor = 3;
    static const quint32 versionPatch = 3;

public slots:
    bool init();
    bool autoLogin();
    void btc();
    void btc(const QString &host);
    void http();
    void http(const QString &url, bool avoidCaches, int threads, int targetTime,
              int rampUpTime, int slotLength);
    void upnp(const bool mediaServerSearch);
    void upnp();
    void packetTrains();
    void packetTrains(const QString host,
                      quint16 port,
                      quint16 packetSize,
                      quint16 trainLength,
                      quint8 iterations,
                      quint64 rateMin,
                      quint64 rateMax,
                      quint64 delay);
    void dnslookup();
    void reverseDnslookup();
    void ping();
    void ping(const QString &url,
              const quint32 &count,
              const quint32 &interval,
              const quint32 &receiveTimeout,
              const int &ttl,
              const quint16 &destinationPort,
              const quint16 &sourcePort,
              const quint32 &payload,
              const ping::PingType &type);
    void traceroute();
    void traceroute(const QString &url,
                    const quint32 &count,
                    const quint32 &interval,
                    const quint32 &receiveTimeout,
                    const quint16 &destinationPort,
                    const quint16 &sourcePort,
                    const quint32 &payload,
                    const ping::PingType type);
    void wifiLookup();
    void measureIt();

signals:
    void statusChanged();

private:
    class Private;
    Private *d;
};

#endif // CLIENT_H
