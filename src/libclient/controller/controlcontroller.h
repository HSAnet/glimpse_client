#ifndef CONTROLCONTROLLER_H
#define CONTROLCONTROLLER_H

#include "../export.h"

#include <QObject>

class NetworkManager;
class Scheduler;
class Settings;

class CLIENT_API ControlController : public QObject
{
    Q_OBJECT

public:
    ControlController(QObject* parent = 0);
    ~ControlController();

    bool init(NetworkManager* networkManager,
              Scheduler* scheduler,
              Settings* settings);

signals:

protected:
    class Private;
    Private* d;
};

#endif // CONTROLCONTROLLER_H
