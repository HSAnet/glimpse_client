#include "client.h"
#include "tests/test.h"
#include "webrequester.h"
#include "measurement/ping/ping.h"
#include "connectiontester.h"
#include "network/requests/requests.h"
#include "scheduler/scheduler.h"
#include "scheduler/schedulermodel.h"
#include "report/reportscheduler.h"
#include "report/reportmodel.h"
#include "task/taskexecutor.h"
#include "controller/reportcontroller.h"
#include "settings.h"
#include "storage/storagepaths.h"
#include "log/logger.h"

#include "qtquick2applicationviewer.h"

#include <QGuiApplication>
#include <QQuickView>
#include <QQmlContext>
#include <QQmlEngine>
#include <QtQml>
#include <QUrl>

#ifdef Q_OS_ANDROID
#include "statusbarhelper.h"
#include "androidimageprovider.h"
#include "androidprocessmodel.h"
#elif !defined(Q_OS_IOS)
#include <QApplication>
#include "desktopstatusbarhelper.h"
#endif // Q_OS_ANDROID

#ifdef Q_OS_OSX
#include <macprocessmodel.h>
#include <macimageprovider.h>
#endif

#define HAVE_BREAKPAD
#ifdef HAVE_BREAKPAD
#include "crashhandler.h"
#endif

#ifdef Q_OS_IOS
#include <QtPlugin>
Q_IMPORT_PLUGIN(QtQuick2Plugin)
Q_IMPORT_PLUGIN(QtQuick2WindowPlugin)
Q_IMPORT_PLUGIN(QtQuickControlsPlugin)
#endif

LOGGER(main)

class Time : public QObject
{
    Q_OBJECT

public:
    Time(QObject* parent = 0) : QObject(parent) {
    }

    Q_INVOKABLE int restart() {
        return time.restart();
    }

    Q_INVOKABLE int elapsed() const {
        return time.elapsed();
    }

protected:
    QTime time;
};

int main(int argc, char* argv[])
{
    QCoreApplication::setOrganizationDomain("de.hsaugsburg.informatik");
    QCoreApplication::setOrganizationName("HS Augsburg");
    QCoreApplication::setApplicationName("mPlaneClient");

#if defined(Q_OS_ANDROID) || defined(Q_OS_IOS)
    QGuiApplication app(argc, argv);
#else
    QApplication app(argc, argv);
    app.setQuitOnLastWindowClosed(true);
#endif

#ifdef HAVE_BREAKPAD
    QDir crashdumpDir = StoragePaths().crashDumpDirectory();
    if (!crashdumpDir.exists()) {
        if (!QDir::root().mkpath(crashdumpDir.absolutePath())) {
            LOG_ERROR(QString("Failed to create crashdump directory: %1").arg(crashdumpDir.absolutePath()));
        } else {
            LOG_INFO("Crashdump directory created");
        }
    }

    CrashHandler crashHandler;
    crashHandler.init(crashdumpDir.absolutePath());
#endif // HAVE_BREAKPAD

    qmlRegisterUncreatableType<AbstractTest>("mplane", 1, 0, "AbstractTest", "abstract class");
    qmlRegisterUncreatableType<Client>("mplane", 1, 0, "Client", "This is a singleton");

    qmlRegisterUncreatableType<Scheduler>("mplane", 1, 0, "Scheduler", "uncreatable type");
    qmlRegisterType<SchedulerModel>("mplane", 1, 0, "SchedulerModel");

    qmlRegisterUncreatableType<TaskExecutor>("mplane", 1, 0, "TaskExecutor", "uncreatable type");
    qmlRegisterUncreatableType<ReportController>("mplane", 1, 0, "ReportController", "uncreatable type");
    qmlRegisterUncreatableType<ReportScheduler>("mplane", 1, 0, "ReportScheduler", "uncreatable type");
    qmlRegisterType<ReportModel>("mplane", 1, 0, "ReportModel");

    // Common objects
    qmlRegisterType<TimingInformation>("mplane", 1, 0, "TimingInformation");

    // Requests
    qmlRegisterUncreatableType<Request>("mplane", 1, 0, "Request", "abstract class");
    qmlRegisterType<RegisterDeviceRequest>("mplane", 1, 0, "RegisterDeviceRequest");
    qmlRegisterType<ManualRequest>("mplane", 1, 0, "TestRequest");
    qmlRegisterType<UserRegisterRequest>("mplane", 1, 0, "UserRegisterRequest");
    qmlRegisterType<LoginRequest>("mplane", 1, 0, "LoginRequest");
    qmlRegisterType<GetConfigRequest>("mplane", 1, 0, "GetConfigRequest");

    // Responses
    qmlRegisterUncreatableType<Response>("mplane", 1, 0, "Response", "abstract class");
    qmlRegisterType<RegisterUserResponse>("mplane", 1, 0, "UserRegisterResponse");
    qmlRegisterType<LoginResponse>("mplane", 1, 0, "LoginResponse");
    qmlRegisterType<RegisterDeviceResponse>("mplane", 1, 0, "RegisterDeviceResponse");
    qmlRegisterType<GetConfigResponse>("mplane", 1, 0, "GetConfigResponse");

    qmlRegisterType<Settings>("mplane", 1, 0, "Settings");
    qmlRegisterType<WebRequester>("mplane", 1, 0, "WebRequester");
    qmlRegisterType<Ping>("mplane", 1, 0, "Ping");
    qmlRegisterType<ConnectionTester>("mplane", 1, 0, "ConnectionTester");
    qmlRegisterType<ConnectionTesterModel>("mplane", 1, 0, "ConnectionTesterModel");
    qmlRegisterType<Time>("mplane", 1, 0, "Time");

#ifdef Q_OS_ANDROID
    qmlRegisterType<AndroidProcessModel>("mplane", 1, 0, "ProcessModel");
#endif // Q_OS_ANDROID

#ifdef Q_OS_OSX
    qmlRegisterType<MacProcessModel>("mplane", 1, 0, "ProcessModel");
#endif // Q_OS_MAC

    QtQuick2ApplicationViewer view;

    QQmlEngine* engine = view.engine();
    // Allow QFileSelector to be automatically applied on qml scripting
    QQmlFileSelector selector;
    engine->setUrlInterceptor(&selector);

    QQmlContext* rootContext = view.rootContext();
    rootContext->setContextProperty("client", Client::instance());
#ifdef Q_OS_ANDROID
    view.engine()->addImageProvider("android", new AndroidImageProvider);
    rootContext->setContextProperty("statusBar", new StatusBarHelper(&view));
#elif defined(Q_OS_IOS)
    // TODO: iOS Code
#else
    rootContext->setContextProperty("statusBar", new DesktopStatusBarHelper(&view));
#endif // Q_OS_ANDROID

#ifdef Q_OS_OSX
    view.engine()->addImageProvider("android", new MacImageProvider);
#endif // Q_OS_MAC

#ifdef Q_OS_IOS
    view.addImportPath(QStringLiteral("imports/qml"));
#endif

    view.setMainQmlFile(QStringLiteral("qml/main.qml"));
    view.showExpanded();

    int returnCode = app.exec();

    // Cleanly shutdown
    Client::instance()->deleteLater();
    QTimer::singleShot(1, &app, SLOT(quit()));
    app.exec();

    return returnCode;
}

#include "main.moc"
