#ifndef CONTROLCONTROLLER_H
#define CONTROLCONTROLLER_H

#include "controller.h"

class NetworkManager;
class Scheduler;
class Settings;

class CLIENT_API TaskController : public Controller
{
    Q_OBJECT

public:
    TaskController(QObject* parent = 0);
    ~TaskController();

    bool init(NetworkManager* networkManager,
              Scheduler* scheduler,
              Settings* settings);

    // Controller interface
    Status status() const;
    QString errorString() const;

    Q_INVOKABLE void fetchTasks();

signals:

protected:
    class Private;
    Private* d;
};

#endif // CONTROLCONTROLLER_H
