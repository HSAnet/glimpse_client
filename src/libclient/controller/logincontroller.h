#ifndef LOGINCONTROLLER_H
#define LOGINCONTROLLER_H

#include "controller.h"

class NetworkManager;
class Settings;

class CLIENT_API LoginController : public Controller
{
    Q_OBJECT
    Q_PROPERTY(bool loggedIn READ isLoggedIn NOTIFY loggedInChanged)
    Q_PROPERTY(bool registeredDevice READ registeredDevice NOTIFY registeredDeviceChanged)

public:
    explicit LoginController(QObject *parent = 0);
    ~LoginController();

    bool isLoggedIn() const;
    bool registeredDevice() const;

    Q_INVOKABLE void anonymousRegistration();
    Q_INVOKABLE void registration(const QString &username, const QString &password);
    Q_INVOKABLE void login();
    Q_INVOKABLE void checkApiKey();

    // Controller interface
    Status status() const;
    QString errorString() const;

    bool init(NetworkManager *networkManager, Settings *settings);

signals:
    void registrationFinished();
    void loginFinished();
    void error();

    void loggedInChanged();
    void registeredDeviceChanged();

protected:
    class Private;
    Private *d;
};

#endif // LOGINCONTROLLER_H
