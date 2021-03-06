#ifndef CONFIGCONTROLLER_H
#define CONFIGCONTROLLER_H

#include "controller.h"

class NetworkManager;
class Settings;

class CLIENT_API ConfigController : public Controller
{
    Q_OBJECT

public:
    explicit ConfigController(QObject *parent = 0);
    ~ConfigController();

    // Controller interface
    Status status() const;
    QString errorString() const;

    bool init(NetworkManager *networkManager, Settings *settings);

public slots:
    void update();

protected:
    class Private;
    Private *d;
};

#endif // CONFIGCONTROLLER_H
