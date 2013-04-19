#include "mainwindow.h"
#include "discovery.h"

#include <QApplication>
#include <QStyleFactory>
#include <QDebug>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

#ifdef Q_OS_ANDROID
    app.setStyle(QStyleFactory::create("Android"));
    //app.setStyle(QStyleFactory::create("Fusion"));
#endif // Q_OS_ANDROID

    // Initialize the client instance
    Client* client = Client::instance();

    MainWindow mainWindow;
    mainWindow.setClient(client);
    mainWindow.show();

    client->init();
    client->registerWithDiscovery();

    return app.exec();
}
