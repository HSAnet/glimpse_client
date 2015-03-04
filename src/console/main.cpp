#include "consoletools.h"
#include "client.h"
#include "log/logger.h"
#include "settings.h"
#include "controller/logincontroller.h"
#include "controller/taskcontroller.h"
#include "controller/configcontroller.h"
#include "types.h"

#include "webrequester.h"
#include "network/requests/registerdevicerequest.h"
#include "network/responses/registerdeviceresponse.h"
#include "log/filelogger.h"
#include <QTextStream>
#include <QCoreApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QTimer>
#include <QDebug>

#ifdef Q_OS_UNIX
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <unistd.h>
#include <errno.h>
#endif // Q_OS_UNIX

LOGGER(main);

struct LoginData
{
    enum Type
    {
        AnonymousRegister,
        Register,
        Login
    };

    Type type;
    QString userId;
    QString password;
};

class DeviceRegistrationWatcher : public QObject
{
    Q_OBJECT

public:
    DeviceRegistrationWatcher(LoginController *controller, bool quit)
    : m_controller(controller)
    , m_quit(quit)
    {
        connect(controller, SIGNAL(finished()), this, SLOT(onLoginOrRegistrationFinished()));
        connect(&m_requester, SIGNAL(statusChanged(WebRequester::Status)), this, SLOT(onStatusChanged(WebRequester::Status)));

        m_requester.setRequest(&m_request);
        m_requester.setResponse(&m_response);
    }

private slots:
    void onLoginOrRegistrationFinished()
    {
        // Already registered?
        if (m_controller->registeredDevice())
        {
            if (m_quit)
            {
                qApp->quit();
            }
            return;
        }

        LOG_INFO("Automatically registering device");

        m_requester.setUrl(QString("https://%1").arg(Client::instance()->settings()->config()->supervisorAddress()));
        m_requester.start();
    }

    void onStatusChanged(WebRequester::Status status)
    {
        switch (status)
        {
        case WebRequester::Error:
            LOG_ERROR(QString("Device registration failed, quitting. (%1)").arg(m_requester.errorString()));
            qApp->quit();
            break;

        case WebRequester::Finished:
            LOG_INFO("Device successfully registered");
            if (m_quit)
            {
                qApp->quit();
            }
            break;

        default:
            break;
        }
    }

protected:
    LoginController *m_controller;

    WebRequester m_requester;
    RegisterDeviceRequest m_request;
    RegisterDeviceResponse m_response;
    bool m_quit; // quit after registration
};

class LoginWatcher : public QObject
{
    Q_OBJECT
public:
    LoginWatcher(LoginController *controller)
    : m_controller(controller)
    {
        connect(controller, SIGNAL(statusChanged()), this, SLOT(onStatusChanged()));
    }

private slots:
    void onStatusChanged()
    {
        switch (m_controller->status())
        {
        case LoginController::Error:
            LOG_ERROR("Login/Registration failed, quitting.");
            qApp->quit();
            break;

        case LoginController::Finished:
            LOG_INFO("Login successful");
            deleteLater();
            break;

        default:
            break;
        }
    }

protected:
    LoginController *m_controller;
};

class ConfigWatcher : public QObject
{
    Q_OBJECT
public:
    ConfigWatcher(Client *client, LoginData loginData)
    : m_client(client)
    , m_loginData(loginData)
    {
        connect(m_client->configController(), SIGNAL(finished()), this, SLOT(onStatusChanged()));
    }

private slots:
    void onStatusChanged()
    {
        switch (m_client->configController()->status())
        {
        case ConfigController::Error:
            LOG_ERROR("Getting config failed, quitting.");
            qApp->quit();
            break;

        case ConfigController::Finished:
            LOG_DEBUG("Getting config successful");
            deleteLater();

            if (!m_loginData.password.isEmpty())
            {
                switch (m_loginData.type)
                {
                case LoginData::Register:
                    m_client->loginController()->registration(m_loginData.userId, m_loginData.password);
                    break;

                case LoginData::Login:
                    m_client->settings()->setUserId(m_loginData.userId);
                    m_client->settings()->setPassword(m_loginData.password);
                    m_client->loginController()->login();
                    break;
                default:
                    break;
                }
            }
            else if (m_loginData.type == LoginData::AnonymousRegister)
            {
                m_client->loginController()->anonymousRegistration();
            }
            else
            {
                if (!m_client->autoLogin())
                {
                    LOG_ERROR("No login data found for autologin");
                    qApp->quit();
                }
            }
            break;

        default:
            break;
        }
    }

protected:
    Client *m_client;
    LoginData m_loginData;
};

int main(int argc, char *argv[])
{
    QCoreApplication::setOrganizationDomain("de.hsaugsburg.informatik");
    QCoreApplication::setOrganizationName("HS-Augsburg");
    QCoreApplication::setApplicationName("mPlaneClient");
    QCoreApplication::setApplicationVersion(Client::version());

    QCoreApplication app(argc, argv);

    Logger::addAppender(new FileLogger);

    QTextStream out(stdout);

    QCommandLineParser parser;
    parser.setApplicationDescription("GLIMPSE console client\r\n\r\n"
                                     "Website: http://www.measure-it.net\r\nCopyright: Revised BSD License");
    parser.addHelpOption();
    parser.addVersionOption();

#ifdef Q_OS_UNIX
    QCommandLineOption userOption("user", "Run as user", "user");
    parser.addOption(userOption);

    //QCommandLineOption groupOption("group", "Run as group", "group");
    //parser.addOption(groupOption);

    QCommandLineOption daemonOption("daemon", "Run as daemon");
    parser.addOption(daemonOption);

    //QCommandLineOption pidFileOption("pidfile", "Set the pidfile", "path");
    //parser.addOption(pidFileOption);

#endif // Q_OS_UNIX

    QCommandLineOption registerAnonymous("register-anonymous", "Register anonymous on server and exit application");
    parser.addOption(registerAnonymous);

    QCommandLineOption registerOption("register", "Register on server and exit application", "mail");
    parser.addOption(registerOption);

    QCommandLineOption loginOption("login", "Login on server and exit application", "mail");
    parser.addOption(loginOption);

    QCommandLineOption passiveOption("passive", "Passive probe which does not receive tasks", "0/1");
    parser.addOption(passiveOption);

    QCommandLineOption trafficOption("traffic", "Traffic limit per month (0 to deactivate traffic limit)", "MB");
    parser.addOption(trafficOption);

    parser.process(app);

    if (parser.isSet(registerOption) && parser.isSet(registerAnonymous))
    {
        out << "'--register' and '--register-anonymous' cannot be set on the same time.\n";
        return 1;
    }

    if ((parser.isSet(registerAnonymous) || parser.isSet(registerOption)) && parser.isSet(loginOption))
    {
        out << "'--register(-anonymous)' and '--login' cannot be set on the same time.\n";
        return 1;
    }

    QCommandLineOption *passwordOption = NULL;
    LoginData loginData;
    bool quit = false; // set true if app should exit after login/registration

    if (parser.isSet(registerOption))
    {
        loginData.type = LoginData::Register;
        passwordOption = &registerOption;
        quit = true;

        loginData.userId = parser.value(*passwordOption).toUtf8();
    }

    if (parser.isSet(loginOption))
    {
        loginData.type = LoginData::Login;
        passwordOption = &loginOption;
        quit = true;

        loginData.userId = parser.value(*passwordOption).toUtf8();
    }

    if (parser.isSet(registerAnonymous))
    {
        loginData.type = LoginData::AnonymousRegister;
        quit = true;
    }

    if (passwordOption)
    {
        ConsoleTools tools;

        do
        {
            out << '[' << argv[0] << ']' << " Password: ";
            out.flush();
            loginData.password = tools.readPassword();
        }
        while (loginData.password.isEmpty());
    }

#ifdef Q_OS_UNIX

    if (parser.isSet(userOption))
    {
        QByteArray user = parser.value(userOption).toLatin1();
        passwd *pwd = getpwnam(user.constData());

        if (pwd)
        {
            if (-1 == setuid(pwd->pw_uid))
            {
                out << "Cannot set uid to " << pwd->pw_uid << ": " << strerror(errno) << "\n";
                return 1;
            }
            else
            {
                LOG_DEBUG(QString("User id set to %1 (%2)").arg(pwd->pw_uid).arg(QString::fromLatin1(user)));
            }
        }
        else
        {
            out << "No user named " << QString::fromLatin1(user) << " found\n";
            return 1;
        }
    }

    // NOTE: This should be the last option since this creates a process fork!
    if (parser.isSet(daemonOption))
    {
        pid_t pid = fork();

        if (pid == -1)
        {
            out << "fork() failed: " << strerror(errno) << "\n";
        }
        else if (pid > 0)
        {
            // Fork successful, we're exiting now
            out << "Daemon started with pid " << pid << "\n";
            return 0;
        }
        else
        {
            // Child process
            umask(0);

            pid_t sid = setsid();

            if (sid < 0)
            {
                return 1;
            }

            //chdir("/");

            close(STDIN_FILENO);
            close(STDOUT_FILENO);
            close(STDERR_FILENO);
        }
    }

#endif // Q_OS_UNIX

    // If there may be anything left, we flush it before the client starts
    out.flush();

    // Initialize the client instance
    Client *client = Client::instance();

    LOG_INFO(QString("Glimpse version %1").arg(Client::version()));

    if (parser.isSet(passiveOption))
    {
        int setPassive = parser.value(passiveOption).toInt();
        client->settings()->setPassive((bool) setPassive);
    }

    if (parser.isSet(trafficOption))
    {
        int setTraffic = parser.value(trafficOption).toInt();
        if (setTraffic != 0)
        {
            client->settings()->setAvailableTraffic(setTraffic * 1024 * 1024);
            client->settings()->setAvailableMobileTraffic(setTraffic * 1024 * 1024);
            client->settings()->setTrafficBudgetManagerActive(true);
        }
        else
        {
            client->settings()->setTrafficBudgetManagerActive(false);
        }
    }

    if (!client->init())
    {
        LOG_ERROR("Client initialization failed")
        return 1;
    }

    new LoginWatcher(client->loginController());
    new DeviceRegistrationWatcher(client->loginController(), quit);
    new ConfigWatcher(client, loginData);

    int value = app.exec();
    out << "Application shutting down.\n";

    Client::instance()->deleteLater();
    QTimer::singleShot(1, &app, SLOT(quit()));
    app.exec();

    return value;
}

#include "main.moc"
