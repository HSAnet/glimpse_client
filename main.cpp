#include "mainwindow.h"
#include "discovery.h"

#include <QApplication>
#include <QDebug>

int main(int argc, char* argv[])
{
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
