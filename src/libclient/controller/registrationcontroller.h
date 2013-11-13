#ifndef REGISTRATIONCONTROLLER_H
#define REGISTRATIONCONTROLLER_H

#include "controller.h"

class NetworkManager;
class Settings;

class RegistrationController : public Controller
{
    Q_OBJECT
    Q_PROPERTY(bool registered READ isRegistered NOTIFY registeredChanged)

public:
    explicit RegistrationController(QObject* parent = 0);
    ~RegistrationController();

    bool isRegistered() const;

    Q_INVOKABLE void anonymousRegistration();
    Q_INVOKABLE void registration(const QString& username, const QString& password);

    // Controller interface
    Status status() const;
    QString errorString() const;

    bool init(NetworkManager* networkManager, Settings* settings);

signals:
    void registeredChanged();

protected:
    class Private;
    Private* d;
};

#endif // REGISTRATIONCONTROLLER_H
