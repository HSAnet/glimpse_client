#ifndef CRASHCONTROLLER_H
#define CRASHCONTROLLER_H

#include "controller.h"

class NetworkManager;
class Settings;

class CLIENT_API CrashController : public Controller
{
    Q_OBJECT
    Q_PROPERTY(bool hasCrashReports READ hasCrashReports NOTIFY hasCrashReportsChanged)

public:
    explicit CrashController(QObject *parent = 0);
    ~CrashController();

    bool hasCrashReports() const;

    Q_INVOKABLE void uploadReports();

    // Controller interface
    Status status() const;
    QString errorString() const;

    bool init(NetworkManager* networkManager, Settings* settings);

signals:
    void hasCrashReportsChanged();

protected:
    class Private;
    Private* d;
};

#endif // CRASHCONTROLLER_H
