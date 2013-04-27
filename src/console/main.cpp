#include "client.h"

#include <QCoreApplication>
#include <QDebug>

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);

    // Initialize the client instance
    Client* client = Client::instance();

    if (!client->init()) {
        qDebug() << "Client initialization failed";
        return 1;
    }

    return app.exec();
}
