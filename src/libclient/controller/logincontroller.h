#ifndef LOGINCONTROLLER_H
#define LOGINCONTROLLER_H

#include "controller.h"

class NetworkManager;
class Settings;

class CLIENT_API LoginController : public Controller
{
    Q_OBJECT
    Q_PROPERTY(bool loggedIn READ isLoggedIn NOTIFY loggedInChanged)

public:
    explicit LoginController(QObject* parent = 0);
    ~LoginController();

    bool isLoggedIn() const;

    Q_INVOKABLE void login();

    // Controller interface
    Status status() const;
    QString errorString() const;

    bool init(NetworkManager* networkManager, Settings* settings);

signals:
    void loggedInChanged();

protected:
    class Private;
    Private* d;
};

#endif // LOGINCONTROLLER_H
