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
#include "controller/reportcontroller.h"
#include "settings.h"
#include "crashhandler.h"

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
#else
#include <QApplication>
#include "desktopstatusbarhelper.h"
#endif // Q_OS_ANDROID

#ifdef Q_OS_MAC
#include <macprocessmodel.h>
#include <macimageprovider.h>
#endif

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

    CrashHandler crashHandler;
    crashHandler.init("/home/gri/dump");

#ifdef Q_OS_ANDROID
    QGuiApplication app(argc, argv);
#else
    QApplication app(argc, argv);
    app.setQuitOnLastWindowClosed(true);
#endif

    qmlRegisterUncreatableType<AbstractTest>("mplane", 1, 0, "AbstractTest", "abstract class");
    qmlRegisterUncreatableType<Client>("mplane", 1, 0, "Client", "This is a singleton");

    qmlRegisterUncreatableType<Scheduler>("mplane", 1, 0, "Scheduler", "uncreatable type");
    qmlRegisterType<SchedulerModel>("mplane", 1, 0, "SchedulerModel");

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

#ifdef Q_OS_MAC
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
#else
    rootContext->setContextProperty("statusBar", new DesktopStatusBarHelper(&view));
#endif // Q_OS_ANDROID

#ifdef Q_OS_MAC
    view.engine()->addImageProvider("android", new MacImageProvider);
#endif // Q_OS_MAC

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
