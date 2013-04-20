#include "mainwindow.h"
#include "discovery.h"

#include <QApplication>
#include <QStyleFactory>
#include <QDebug>

int main(int argc, char* argv[])
{
#ifdef Q_OS_ANDROID
    QApplication::setStyle(QStyleFactory::create("android"));
#endif // Q_OS_ANDROID

    QApplication app(argc, argv);

    // Initialize the client instance
    Client* client = Client::instance();

    MainWindow mainWindow;
    mainWindow.setClient(client);
    mainWindow.show();

    client->init();
    client->registerWithDiscovery();

    return app.exec();
}
