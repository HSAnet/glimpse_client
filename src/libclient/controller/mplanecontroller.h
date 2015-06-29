#ifndef MPLANECONTROLLER_H
#define MPLANECONTROLLER_H

#include "controller.h"

class NetworkManager;
class Scheduler;
class Settings;

class CLIENT_API MPlaneController : Controller
{
    Q_OBJECT
public:
    MPlaneController(QObject *parent = 0);
    ~MPlaneController();

    bool init(NetworkManager *networkManager,
              Scheduler *scheduler,
              Settings *settings);

    // Controller interface
    Status status() const;
    QString errorString() const;

public slots:
    void sendCapabilities();
    void fetchSpecifications();

signals:

protected:
    class Private;
    Private *d;
};

#endif // MPLANECONTROLLER_H
