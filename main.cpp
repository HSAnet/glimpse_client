#include "client.h"
#include "discovery.h"

#include <QCoreApplication>
#include <QDebug>

class Test : public QObject
{
    Q_OBJECT

public:
    Test()
    {
        connect(&discovery, SIGNAL(finished()), this, SLOT(finished()));
    }

public slots:
    void finished()
    {
        qDebug() << discovery.data();
    }

public:
    Discovery discovery;
};

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);

    Test t;
    t.discovery.discover();

    // Initialize the client instance
    Client* client = Client::instance();
    client->init();
    client->registerWithDiscovery();

    return app.exec();
}

#include "main.moc"
