#include "client.h"

#include <QGuiApplication>
#include <QQuickView>
#include <QQmlContext>
#include <QtQml>
#include <QUrl>

int main(int argc, char* argv[])
{
    QGuiApplication app(argc, argv);

    qmlRegisterUncreatableType<Client>("mplane", 1, 0, "Client", "This is a singleton");

    QQuickView view;

    QQmlContext* rootContext = view.rootContext();
    rootContext->setContextProperty("client", Client::instance());

    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.setSource( QUrl("qrc:/qml/main.qml") );
    view.show();

    return app.exec();
}
