#ifndef LOGINCONTROLLER_H
#define LOGINCONTROLLER_H

#include "../export.h"

#include <QObject>

class NetworkManager;
class Settings;

class CLIENT_API LoginController : public QObject
{
    Q_OBJECT

public:
    LoginController(QObject* parent = 0);
    ~LoginController();

    enum Status
    {
    };

    bool isLoggedIn() const;

    void login();

    bool init(NetworkManager* networkManager, Settings* settings);

protected:
    class Private;
    Private* d;
};

#endif // LOGINCONTROLLER_H
