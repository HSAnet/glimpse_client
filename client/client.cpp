#include "client.h"
#include "discovery.h"

#include <QUdpSocket>
#include <QBuffer>
#include <QMetaObject>
#include <QMetaEnum>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QTimer>

#include <qjson/parser.h>
#include <QDebug>

const QUrl masterUrl = QUrl("https://localhost:8080/register");
const quint32 magic = 0x1337;

class Client::Private : public QObject
{
    Q_OBJECT

public:
    Private(Client* q)
    : q(q)
    , status(Client::Unregistered)
    , networkAccessManager(new QNetworkAccessManager(q))
    {
        connect(&discovery, SIGNAL(finished()), this, SLOT(onDiscoveryFinished()));
        connect(&managerSocket, SIGNAL(readyRead()), this, SLOT(onDatagramReady()));
        connect(&aliveTimer, SIGNAL(timeout()), this, SLOT(onAliveTimer()));

        aliveTimer.setInterval(45 * 1000);
        aliveTimer.start();
    }

    Client* q;

    // Properties
    Client::Status status;
    RemoteInfoList remoteInfo;
    QNetworkAccessManager* networkAccessManager;
    Discovery discovery;
    QUdpSocket managerSocket;
    QTimer aliveTimer;

    // Functions
    void setStatus(Client::Status status);

    void processDatagram(QBuffer& buffer, const QHostAddress& host, quint16 port);

public slots:
    void onDatagramReady();
    void onAliveTimer();
    void onDiscoveryFinished();
    void onRegisterFinished();
    void onRegisterError(QNetworkReply::NetworkError error);
};

void Client::Private::setStatus(Client::Status status)
{
    if ( this->status == status )
        return;

    this->status = status;
    emit q->statusChanged();
}

void Client::Private::processDatagram(QBuffer& buffer, const QHostAddress &host, quint16 port)
{
    // TODO: byte order checking
    quint32 m;
    buffer.read((char*)&m, sizeof(quint32));

    if (m != magic) {
        qDebug() << "Received invalid packet from" << host.toString() << port;
        return;
    }

    qDebug() << "Received valid packet from" << host.toString() << port;
}

void Client::Private::onDatagramReady()
{
    while (managerSocket.hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(managerSocket.pendingDatagramSize());

        QHostAddress host;
        quint16 port;
        managerSocket.readDatagram(datagram.data(), datagram.size(), &host, &port);

        // Process the datagram using a buffer
        QBuffer buffer(&datagram);
        buffer.open(QIODevice::ReadOnly);
        processDatagram(buffer, host, port);
    }
}

void Client::Private::onAliveTimer()
{
    qint64 bytes = managerSocket.writeDatagram((char*)&magic, sizeof(magic), QHostAddress("62.75.147.195"), 34366);
    qDebug() << "Alive packet with" << bytes << "bytes sent";
}

void Client::Private::onDiscoveryFinished()
{
    q->register_();
}

void Client::Private::onRegisterFinished()
{
    bool ok = false;
    QJson::Parser parser;

    // Parse the reply data
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    QVariant root = parser.parse(reply, &ok);
    if ( ok ) {
        setStatus(Client::Registered);

        RemoteInfoList remotes;
        foreach(const QVariant& entry, root.toList()) {
            RemoteInfo info;
            info.peerAddress = QHostAddress(entry.toString());

            remotes.append(info);
        }

        // Update the remote list
        if (!remotes.isEmpty())
            q->setRemoteInfo(remotes);
    } else
        setStatus(Client::Unregistered);
}

void Client::Private::onRegisterError(QNetworkReply::NetworkError error)
{
    setStatus(Client::Unregistered);
    qDebug() << "Registration error" << error;
}

Client::Client(QObject *parent)
: QObject(parent)
, d(new Private(this))
{
}

Client::~Client()
{
    delete d;
}

Client *Client::instance()
{
    static Client* ins = NULL;

    if ( !ins )
        ins = new Client();

    return ins;
}

bool Client::init()
{
    bool ok = false;
    ok |= d->managerSocket.bind(34366);

    // Cheat ...
    d->onAliveTimer();

    return ok;
}

Client::Status Client::status() const
{
    return d->status;
}

QNetworkAccessManager *Client::networkAccessManager() const
{
    return d->networkAccessManager;
}

void Client::setRemoteInfo(const RemoteInfoList &remoteInfo)
{
    d->remoteInfo = remoteInfo;
}

RemoteInfoList Client::remoteInfo() const
{
    return d->remoteInfo;
}

void Client::registerWithDiscovery()
{
    d->discovery.discover();
}

void Client::register_()
{
    // TODO: Fill data. Available tests, language, etc.
    QByteArray data = "ttl=60&";

    const QMetaObject* meta = &Discovery::staticMetaObject;
    QMetaEnum en = meta->enumerator(meta->indexOfEnumerator("DataType"));

    QHashIterator<Discovery::DataType, QVariant> iter(d->discovery.data());
    while (iter.hasNext()) {
        iter.next();

        // TODO: escapings
        data.append(en.valueToKey(iter.key()));
        data.append('=');
        data.append(iter.value().toString());
        data.append('&');
    }

    QNetworkRequest request(masterUrl);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

    QNetworkReply* reply = d->networkAccessManager->post(request, data);
    reply->ignoreSslErrors(); // TODO: Remove the evil
    connect(reply, SIGNAL(finished()), d, SLOT(onRegisterFinished()));
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), d, SLOT(onRegisterError(QNetworkReply::NetworkError)));
}

void Client::unregister()
{
}

#include "client.moc"
