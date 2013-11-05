#include "qmlmodule.h"

// Classes to be registered
#include "units.h"
#include "client.h"
#include "scheduler/scheduler.h"
#include "scheduler/schedulermodel.h"
#include "task/taskexecutor.h"
#include "report/reportmodel.h"
#include "report/reportscheduler.h"
#include "network/requests/requests.h"
#include "controller/logincontroller.h"
#include "controller/reportcontroller.h"
#include "response.h"
#include "webrequester.h"
#include "connectiontester.h"
#include "settings.h"

#include <QQmlEngine>
#include <QQmlContext>
#include <qqml.h>

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

// Don't make this a const char*, otherwise QtCreator static analyzer
// can't determine the namespace for code-completion
#define MODULE_URI "mplane"

void QmlModule::registerTypes()
{
    qmlRegisterUncreatableType<Units>(MODULE_URI, 1, 0, "Units", "Do not create units");
    qmlRegisterUncreatableType<Client>(MODULE_URI, 1, 0, "Client", "This is a singleton");

    qmlRegisterUncreatableType<Scheduler>(MODULE_URI, 1, 0, "Scheduler", "uncreatable type");
    qmlRegisterType<SchedulerModel>(MODULE_URI, 1, 0, "SchedulerModel");

    qmlRegisterUncreatableType<TaskExecutor>(MODULE_URI, 1, 0, "TaskExecutor", "uncreatable type");
    qmlRegisterUncreatableType<ReportController>(MODULE_URI, 1, 0, "ReportController", "uncreatable type");
    qmlRegisterUncreatableType<ReportScheduler>(MODULE_URI, 1, 0, "ReportScheduler", "uncreatable type");
    qmlRegisterType<ReportModel>(MODULE_URI, 1, 0, "ReportModel");

    // Common objects
    qmlRegisterType<TimingInformation>(MODULE_URI, 1, 0, "TimingInformation");

    // Requests
    qmlRegisterUncreatableType<Request>(MODULE_URI, 1, 0, "Request", "abstract class");
    qmlRegisterType<RegisterDeviceRequest>(MODULE_URI, 1, 0, "RegisterDeviceRequest");
    qmlRegisterType<ManualRequest>(MODULE_URI, 1, 0, "TestRequest");
    qmlRegisterType<UserRegisterRequest>(MODULE_URI, 1, 0, "UserRegisterRequest");
    qmlRegisterType<LoginRequest>(MODULE_URI, 1, 0, "LoginRequest");
    qmlRegisterType<GetConfigRequest>(MODULE_URI, 1, 0, "GetConfigRequest");

    // Responses
    qmlRegisterUncreatableType<Response>(MODULE_URI, 1, 0, "Response", "abstract class");
    qmlRegisterType<RegisterUserResponse>(MODULE_URI, 1, 0, "UserRegisterResponse");
    qmlRegisterType<LoginResponse>(MODULE_URI, 1, 0, "LoginResponse");
    qmlRegisterType<RegisterDeviceResponse>(MODULE_URI, 1, 0, "RegisterDeviceResponse");
    qmlRegisterType<GetConfigResponse>(MODULE_URI, 1, 0, "GetConfigResponse");

    qmlRegisterType<Settings>(MODULE_URI, 1, 0, "Settings");
    qmlRegisterType<WebRequester>(MODULE_URI, 1, 0, "WebRequester");
    qmlRegisterType<ConnectionTester>(MODULE_URI, 1, 0, "ConnectionTester");
    qmlRegisterType<ConnectionTesterModel>(MODULE_URI, 1, 0, "ConnectionTesterModel");
    qmlRegisterType<Time>(MODULE_URI, 1, 0, "Time");
}

void QmlModule::initializeEngine(QQmlEngine *engine)
{
    QQmlContext* context = engine->rootContext();
    context->setContextProperty("units", new Units(engine));
}

#include "qmlmodule.moc"
