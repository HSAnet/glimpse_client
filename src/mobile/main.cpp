#include "client.h"
#include "tests/test.h"
#include "webrequester.h"
#include "ping.h"
#include "connectiontester.h"
#include "discovery.h"

#include <QGuiApplication>
#include <QQuickView>
#include <QQmlContext>
#include <QQmlEngine>
#include <QtQml>
#include <QUrl>

#ifdef Q_OS_ANDROID
#include "statusbarhelper.h"
#else
#include <QSystemTrayIcon>

class DesktopStatusBarHelper : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool visible READ isVisible WRITE setVisible NOTIFY visibleChanged)

public:
    DesktopStatusBarHelper(QObject* parent = 0)
    : QObject(parent)
    {
        m_icon.setIcon( QIcon(":/tray.png") );
        m_icon.show();
    }

    void setVisible(bool visible) {
        if ( m_visible != visible ) {
            m_visible = visible;
            emit visibleChanged(visible);
        }
    }

    bool isVisible() const {
        return m_visible;
    }

signals:
    void visibleChanged(bool visible);

protected:
    bool m_visible;
    QSystemTrayIcon m_icon;
};

#endif // Q_OS_ANDROID

int main(int argc, char* argv[])
{
    QCoreApplication::setOrganizationDomain("de.hsaugsburg.informatik");
    QCoreApplication::setOrganizationName("HS Augsburg");
    QCoreApplication::setApplicationName("mPlaneClient");

    QGuiApplication app(argc, argv);

    qmlRegisterUncreatableType<AbstractTest>("mplane", 1, 0, "AbstractTest", "abstract class");
    qmlRegisterUncreatableType<Client>("mplane", 1, 0, "Client", "This is a singleton");
    qmlRegisterUncreatableType<TestScheduler>("mplane", 1, 0, "TestScheduler", "uncreatable type");

    qmlRegisterUncreatableType<Request>("mplane", 1, 0, "Request", "abstract class");
    qmlRegisterType<ClientInfo>("mplane", 1, 0, "ClientInfoRequest");
    qmlRegisterType<ManualRequest>("mplane", 1, 0, "TestRequest");
    qmlRegisterType<Settings>("mplane", 1, 0, "Settings");
    qmlRegisterType<WebRequester>("mplane", 1, 0, "WebRequester");
    qmlRegisterType<Ping>("mplane", 1, 0, "Ping");
    qmlRegisterType<ConnectionTester>("mplane", 1, 0, "ConnectionTester");
    qmlRegisterType<ConnectionTesterModel>("mplane", 1, 0, "ConnectionTesterModel");
    qmlRegisterType<Discovery>("mplane", 1, 0, "Discovery");

    QQuickView view;

    QQmlEngine* engine = view.engine();
    QObject::connect(engine, SIGNAL(quit()), &app, SLOT(quit()));

    QQmlContext* rootContext = view.rootContext();
    rootContext->setContextProperty("client", Client::instance());
#ifdef Q_OS_ANDROID
    rootContext->setContextProperty("statusBar", new StatusBarHelper(&view));
#else
    rootContext->setContextProperty("statusBar", new DesktopStatusBarHelper(&view));
#endif // Q_OS_ANDROID

    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.setSource( QUrl("qrc:/qml/main.qml") );
    view.show();

    return app.exec();
}

#include "main.moc"
