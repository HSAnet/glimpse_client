#include "client.h"
#include "discovery.h"
#include "types.h"

#include <QUdpSocket>
#include <QBuffer>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QTimer>
#include <QHostInfo>

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QJsonDocument>
#else
#ifdef Q_OS_WIN
#include <QJson/Parser>
#else
#include <qjson/parser.h>
#endif
#endif

#include <QDebug>

const QUrl masterUrl = QUrl("https://mplane.informatik.hs-augsburg.de:16001/register");
const quint32 magic = 0x1337;
const quint8 version = 0;

struct NegotiationPacket {
    quint16 magic;
    quint8 version;

    char host[64];
    quint16 port;

    quint16 ports[10];
};

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

        aliveTimer.setInterval(120 * 1000);
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
    QHostInfo aliveInfo;

    // Functions
    void setStatus(Client::Status status);

    void processDatagram(QBuffer& buffer, const QHostAddress& host, quint16 port);
    void processNegotiation(const NegotiationPacket& packet);

public slots:
    void onDatagramReady();
    void onLookupFinished(const QHostInfo &host);
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
    NegotiationPacket packet;
    buffer.read((char*)&packet, sizeof(packet));

    if (packet.magic != magic) {
        qDebug() << "Received invalid packet from" << host.toString() << port;
        return;
    }

    if (packet.version != version) {
        qDebug() << "Packet version is different" << host.toString() << port;
        return;
    }

    processNegotiation(packet);
}

void Client::Private::processNegotiation(const NegotiationPacket &packet)
{
    qDebug() << "Received valid packet from" << packet.host << packet.port;

    // Post a alive-packet back and hope it gets through
    managerSocket.writeDatagram(QByteArray(), QHostAddress(packet.host), packet.port);
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

void Client::Private::onLookupFinished(const QHostInfo& host)
{
    aliveInfo = host;

    if ( !host.addresses().isEmpty() ) {
        managerSocket.connectToHost(host.addresses().first(), 16000);
        onAliveTimer();
    } else {
        // Wait some seconds before trying again
        QTimer::singleShot(5000, this, SLOT(onAliveTimer()));
    }
}

void Client::Private::onAliveTimer()
{
    if (aliveInfo.addresses().isEmpty()) {
        aliveInfo.lookupHost("mplane.informatik.hs-augsburg.de", this, SLOT(onLookupFinished(QHostInfo)));
        qDebug() << "Looking up alive host";
    } else {
        managerSocket.write(QByteArray());
        qDebug() << "Alive packet sent";
    }
}

void Client::Private::onDiscoveryFinished()
{
    q->register_();
}

void Client::Private::onRegisterFinished()
{
    bool ok = false;
    QString errorMessage;

    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    QJsonParseError error;

    // Parse the reply data
    QVariant root = QJsonDocument::fromJson(reply->readAll(), &error).toVariant();
    ok = (error.error = QJsonParseError::NoError);
    errorMessage = error.errorString();
#else
    QJson::Parser parser;
    QVariant root = parser.parse(reply, &ok);
    errorMessage = parser.errorString();
#endif
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

        setStatus(Client::Registered);
    } else {
        qDebug() << "JSon parser error:" << errorMessage;

        setStatus(Client::Unregistered);
    }
}

void Client::Private::onRegisterError(QNetworkReply::NetworkError error)
{
    setStatus(Client::Unregistered);

    qDebug() << "Registration error" << enumToString(QNetworkReply, "NetworkError", error);
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

QAbstractSocket *Client::managerSocket() const
{
    return &d->managerSocket;
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

    /*const QMetaObject* meta = &Discovery::staticMetaObject;
    QMetaEnum en = meta->enumerator(meta->indexOfEnumerator("DataType"));

    QHashIterator<Discovery::DataType, QVariant> iter(d->discovery.data());
    while (iter.hasNext()) {
        iter.next();

        // TODO: escapings
        data.append(en.valueToKey(iter.key()));
        data.append('=');
        data.append(iter.value().toString());
        data.append('&');
    }*/

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
