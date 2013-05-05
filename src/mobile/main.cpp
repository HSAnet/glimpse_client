#include "client.h"
#include "tests/test.h"

#include <QGuiApplication>
#include <QQuickView>
#include <QQmlContext>
#include <QQmlEngine>
#include <QtQml>
#include <QUrl>

#ifdef Q_OS_ANDROID
#include "statusbarhelper.h"
#endif // Q_OS_ANDROID

int main(int argc, char* argv[])
{
    QGuiApplication app(argc, argv);

    qmlRegisterUncreatableType<AbstractTest>("mplane", 1, 0, "AbstractTest", "abstract class");
    qmlRegisterUncreatableType<Client>("mplane", 1, 0, "Client", "This is a singleton");
    qmlRegisterUncreatableType<TestScheduler>("mplane", 1, 0, "TestScheduler", "uncreatable type");

    QQuickView view;

    QQmlEngine* engine = view.engine();
    QObject::connect(engine, SIGNAL(quit()), &app, SLOT(quit()));

    QQmlContext* rootContext = view.rootContext();
    rootContext->setContextProperty("client", Client::instance());
#ifdef Q_OS_ANDROID
    rootContext->setContextProperty("statusBar", new StatusBarHelper(&view));
#endif // Q_OS_ANDROID

    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.setSource( QUrl("qrc:/qml/main.qml") );
    view.show();

    return app.exec();
}
