#include "mainwindow.h"
#include "discovery.h"

#include <QApplication>
#include <QStyleFactory>
#include <QDebug>

int main(int argc, char* argv[])
{
    QCoreApplication::setOrganizationDomain("de.hsaugsburg.informatik");
    QCoreApplication::setOrganizationName("HS Augsburg");
    QCoreApplication::setApplicationName("mPlaneClient");

#ifdef Q_OS_ANDROID
    QApplication::setStyle(QStyleFactory::create("android"));
#endif // Q_OS_ANDROID

    QApplication app(argc, argv);

    // Initialize the client instance
    Client* client = Client::instance();

    if (!client->init()) {
        qDebug() << "Client initialization failed";
        return 1;
    }

    MainWindow mainWindow;
    mainWindow.setClient(client);
    mainWindow.show();

    return app.exec();
}
