#include "client.h"
#include "discovery.h"
#include "network/requests/registerdevicerequest.h"
#include "network/requests/manualrequest.h"
#include "networkhelper.h"

#include <QUdpSocket>
#include <QBuffer>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QTimer>
#include <QHostInfo>
#include <QDataStream>

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#else
#ifdef Q_OS_WIN
#include <QJson/Parser>
#else
#include <qjson/parser.h>
#endif
#endif

#include <QDebug>

const QUrl masterUrl = QUrl("https://141.82.49.82:5105");

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

        scheduler.setManagerSocket(&managerSocket);

        aliveTimer.setInterval(30 * 1000);
        aliveTimer.start();
    }

    Client* q;

    // Properties
    Client::Status status;
    QNetworkAccessManager* networkAccessManager;
    Discovery discovery;
    QUdpSocket managerSocket;
    QTimer aliveTimer;
    QHostInfo aliveInfo;

    TestScheduler scheduler;
    Settings settings;

    QHostAddress lastLocalIp;

    // Functions
    void processDatagram(const QByteArray &datagram, const QHostAddress& host, quint16 port);
    void processClientInfoRequest();
    void processPeerRequest();

    void sendClientInfo();
    void sendPeerResponse(const QHostAddress &host, quint16 port);
    void sendPeerRequest(bool manual = false);

public slots:
    void onDatagramReady();
    void onLookupFinished(const QHostInfo &host);
    void onAliveTimer();
    void onDiscoveryFinished();

    void onRegisterFinished();
    void onRegisterError(QNetworkReply::NetworkError error);

    void onPeerRequestFinished();
    void onPeerRequestError(QNetworkReply::NetworkError error);
};

void Client::Private::processDatagram(const QByteArray& datagram, const QHostAddress &host, quint16 port)
{
    // Master server
    if ( aliveInfo.addresses().contains(host) ) {
        const RequestType* type = (const RequestType*)datagram.constData();

        qDebug() << Q_FUNC_INFO << *type;

        switch(*type) {
        case ClientInfoRequest:
            processClientInfoRequest();
            break;

        case PeerRequest:
            sendPeerRequest();
            break;

        case RegisteredClientResponse:
            q->setStatus(Client::Registered);
            break;

        default:
            qDebug() << "Received unknown request from master" << host.toString() << port;
        }
    } else {
        AbstractTest* currentTest = scheduler.currentTest();
        if ( currentTest ) {
            currentTest->processDatagram(datagram, host, port);
        } else {
            const RequestType* type = (const RequestType*)datagram.constData();

            switch(*type) {
            case PeerResponse:
                qDebug() << "Received peer response from" << host.toString() << port;
                break;

            default:
                qDebug() << "Received invalid datagram from" << host.toString() << port;
            }
        }
    }
}

void Client::Private::processClientInfoRequest()
{
    qDebug() << Q_FUNC_INFO;

    q->setStatus(Client::Unregistered);

    // Get some information
    discovery.discover();
}

void Client::Private::processPeerRequest()
{
    qDebug() << Q_FUNC_INFO;
}

void Client::Private::sendClientInfo()
{
    Discovery::DiscoveryHash upnpData = discovery.data();

    QHostAddress myIp = upnpData.contains(Discovery::LanIpAddress) ? QHostAddress(upnpData.value(Discovery::LanIpAddress).toString())
                                                                   : NetworkHelper::localIpAddress();
    QString localIp = QString("%1:%2").arg(myIp.toString()).arg(1337);

    // Prepare upnp data
    QVariantMap upnp;
    QHashIterator<Discovery::DataType, QVariant> iter(upnpData);
    while ( iter.hasNext() ) {
        iter.next();

        QString name = enumToString(Discovery, "DataType", iter.key());
        name = name.replace(QRegExp("([A-Z])"), "-\\1").toLower();
        name.remove(0, 1);

        upnp.insert(name, iter.value());
    }

    RegisterDeviceRequest info;
    info.setDeviceId(settings.deviceId());
    //info.setUpnp(upnp);
    QByteArray data = QJsonDocument::fromVariant(info.toVariant()).toJson();

    QUrl url = masterUrl;
    url.setPath("/info");

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "text/json");

    QNetworkReply* reply = networkAccessManager->post(request, data);
    reply->ignoreSslErrors(); // TODO: Remove the evil
    connect(reply, SIGNAL(finished()), this, SLOT(onRegisterFinished()));
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(onRegisterError(QNetworkReply::NetworkError)));
}

void Client::Private::sendPeerResponse(const QHostAddress &host, quint16 port)
{
    qDebug() << Q_FUNC_INFO;

    RequestType type = PeerResponse;

    managerSocket.writeDatagram((const char*)&type, sizeof(type), host, port);
}

void Client::Private::sendPeerRequest(bool manual)
{
    ManualRequest r;
    r.setDeviceId(settings.deviceId());
    QByteArray data = QJsonDocument::fromVariant(r.toVariant()).toJson();

    QUrl url = masterUrl;
    if (manual)
        url.setPath("/manualrequest");
    else
        url.setPath("/request");

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "text/json");

    QNetworkReply* reply = networkAccessManager->post(request, data);
    reply->setProperty("manual", manual);
    reply->ignoreSslErrors();
    connect(reply, SIGNAL(finished()), this, SLOT(onPeerRequestFinished()));
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(onPeerRequestError(QNetworkReply::NetworkError)));
}

void Client::Private::onDatagramReady()
{
    while (managerSocket.hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(managerSocket.pendingDatagramSize());

        QHostAddress host;
        quint16 port;
        managerSocket.readDatagram(datagram.data(), datagram.size(), &host, &port);

        // Process the datagram
        processDatagram(datagram, host, port);
    }
}

void Client::Private::onLookupFinished(const QHostInfo& host)
{
    aliveInfo = host;

    if ( !host.addresses().isEmpty() ) {
        qDebug() << "Alive host found:" << host.addresses().first();
        onAliveTimer();
    } else {
        // Wait some seconds before trying again
        QTimer::singleShot(5000, this, SLOT(onAliveTimer()));
    }
}

void Client::Private::onAliveTimer()
{
    if (aliveInfo.addresses().isEmpty()) {
        aliveInfo.lookupHost(masterUrl.host(), this, SLOT(onLookupFinished(QHostInfo)));
        qDebug() << "Looking up alive host";
    } else {
        QString sessionId = settings.sessionId();
        if (sessionId.isEmpty())
            return;

        QStringList srcIp;
        srcIp.append( NetworkHelper::localIpAddress().toString() );

        QVariantMap map;
        map.insert("type", "keepalive");
        map.insert("session_id", sessionId);
        map.insert("src_ip", srcIp);
        map.insert("src_port", 1337);

        QByteArray data = QJsonDocument::fromVariant(map).toJson();

        managerSocket.writeDatagram(data, aliveInfo.addresses().first(), 5105);
        qDebug() << "Alive packet sent";
    }
}

void Client::Private::onDiscoveryFinished()
{
    qDebug() << "UPNP discovery finished:" << discovery.data();

    sendClientInfo();
}

void Client::Private::onRegisterFinished()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());

    if (reply->error() == QNetworkReply::NoError)
        q->setStatus(Client::Registered);
    else
        q->setStatus(Client::Unregistered);

    reply->deleteLater();
}

void Client::Private::onRegisterError(QNetworkReply::NetworkError error)
{
    q->setStatus(Client::Unregistered);

    qDebug() << "Registration error" << enumToString(QNetworkReply, "NetworkError", error);
}

void Client::Private::onPeerRequestFinished()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());

    if (reply->error() == QNetworkReply::NoError) {
        bool manual = reply->property("manual").toBool();

        // TODO: What to do when it was a manual request?
        if ( manual )
            return;

        QJsonParseError error;
        QJsonDocument document = QJsonDocument::fromJson(reply->readAll(), &error);

        if ( error.error == QJsonParseError::NoError ) {
            QJsonObject root = document.object();
            bool isMaster = root.value("master").toBool();
            QString testName = root.value("test").toString();
            QJsonArray peers = root.value("peers").toArray();

            qDebug() << "Test request:" << testName << (isMaster ? "master" : "not master");

            PeerList peerList;

            foreach(QJsonValue value, peers) {
                qDebug() << "Received peer:" << value.toString();

                QStringList values = value.toString().split(':');
                if ( values.size() == 2) {
                    QHostAddress host(values.at(0));
                    quint16 port = values.at(1).toUInt();

                    peerList.append(Peer(host, port));
                    sendPeerResponse(host, port);
                }
                else
                    qDebug() << Q_FUNC_INFO << "not an ip with port:" << value.toString();
            }

            scheduler.enqueue( TestInfo(testName, peerList, isMaster) );
        } else {
            qDebug() << Q_FUNC_INFO << "Json error:" << error.errorString();
        }
    } else {
        // Hmm?
    }

    reply->deleteLater();
}

void Client::Private::onPeerRequestError(QNetworkReply::NetworkError error)
{
    qDebug() << "Peer request error" << enumToString(QNetworkReply, "NetworkError", error);
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
    ok = d->managerSocket.bind(1337);

    d->settings.init();

    // Cheat ...
    d->onAliveTimer();

    return ok;
}

void Client::setStatus(Client::Status status)
{
    if ( d->status == status )
        return;

    d->status = status;
    emit statusChanged();
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

TestScheduler *Client::scheduler() const
{
    return &d->scheduler;
}

Settings *Client::settings() const
{
    return &d->settings;
}

void Client::requestTest()
{
    d->sendPeerRequest(true);
}

void Client::speedTest()
{
    TestInfo info("speedtest", PeerList(), true);
    d->scheduler.enqueue(info);
}

#include "client.moc"
