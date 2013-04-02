#include "connectiontester.h"

#include <QNetworkConfigurationManager>
#include <QNetworkConfiguration>

#include <QNetworkInterface>
#include <QCoreApplication>
#include <QDebug>

class ConnectionTester::Private
{
    Q_DECLARE_TR_FUNCTIONS(ConnectionTester::Private)

public:
    Private(ConnectionTester* q)
    : q(q)
    , result(ConnectionTester::Offline)
    {
    }

    ConnectionTester* q;

    ConnectionTester::ResultType result;

    void checkInterfaces();
    void checkSlow();
};

void ConnectionTester::Private::checkInterfaces()
{
    QNetworkConfigurationManager mgr;

    if ( !mgr.isOnline() ) {
        qDebug() << "Offline. Checking why ...";

        emit q->message(tr("Offline."), ConnectionTester::Info);
    } else {
        emit q->message(tr("Online."), ConnectionTester::Info);
    }


    foreach(const QNetworkConfiguration& config, mgr.allConfigurations()) {
        // Ignore unknown and bluetooth devices
        if (config.bearerType() == QNetworkConfiguration::BearerUnknown ||
            config.bearerType() == QNetworkConfiguration::BearerBluetooth )
        {
            continue;
        }

        qDebug() << config.bearerTypeName() << config.name() << config.state();
    }
}

void ConnectionTester::Private::checkSlow()
{
    QNetworkConfigurationManager mgr;

    QNetworkConfiguration config = mgr.defaultConfiguration();
}

ConnectionTester::ConnectionTester(QObject *parent)
: QObject(parent)
, d(new Private(this))
{
}

ConnectionTester::~ConnectionTester()
{
    delete d;
}

void ConnectionTester::start()
{
    d->checkInterfaces();
}

ConnectionTester::ResultType ConnectionTester::result() const
{
    return d->result;
}
