#ifndef HTTPMANAGER_H
#define HTTPMANAGER_H

#include "parser.h"

#include <QObject>
#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QFile>
#include <QHostInfo>

class UPnPHandler :public QObject
{
    Q_OBJECT

public:
    enum DownloadThreadStatus
    {
        Inactive,
        ConnectingTCP,
        ConnectedTCP,
        AwaitingFirstByte,
        DownloadInProgress,
        FinishedSuccess,
        FinishedError
    };

    UPnPHandler();
    ~UPnPHandler();

    int init(QUrl remoteUrl, QString descriptionUrl, QString eventSubUrl, QString controlUrl, QString serviceType);

    QNetworkAccessManager * networkAccessManager() const;
    void setNetworkAccessManager(QNetworkAccessManager *networkAccessManager);
    bool parseXML(QByteArray ba);

    QUrl GETUrl() const;
    void setGETUrl(const QUrl &GETUrl);

    QNetworkRequest GETrequest() const;
    void setGETrequest(const QNetworkRequest &GETrequest);

    QFile *file() const;
    void setFile(const QString filename);

    QUrl subscribeUrl() const;
    void setSubscribeUrl(const QUrl &subscribeUrl);

    QUrl actionUrl() const;
    void setActionUrl(const QUrl &actionUrl);

    static const int tcpConnectTimeout = 10000;
    static const int firstByteReceivedTimeout = 10000;
    static const int defaultPort = 80;
    static const int threadnum = 1;

    QUrl remoteUrl() const;
    void setRemoteUrl(const QUrl &url);

    QHostInfo getServer() const;
    void setServer(const QHostInfo &value);

    QString servicetype() const;
    void setServicetype(const QString &servicetype);

    QList<QMap<QString, QString> > foundContent() const;

    QList<QUrl> ownUrls() const;
    void setOwnUrls(const QList<QUrl> &ownUrls);

public slots:
    void startGet();
    void GETreadyRead();
    void subscribe();
//    void browse();
    void sendRequest();
    void startAction();
    void setupTCPSocket(const QHostInfo &server);
    void read();
    void disconnectionHandling();
    void startTCPConnection();
    void TCPConnectionTracking(bool success);
    void downloadStartedTracking(bool success);
    void tidyUp();
    void handleContent();

signals:
    void subscribed();
    void firstByteReceived(bool success);
    void TCPConnected(bool success);
    void TCPDisconnected();
    void connectTCP();
    void startDownload();
    void readyToParse();
    void handlingDone();

private:
    QString m_servicetype;
    DownloadThreadStatus tStatus;
    Parser *m_parser;
    QTcpSocket *m_socket;
    QNetworkAccessManager *m_networkAccessManager;
    QNetworkReply *m_GETReply;
    QNetworkReply *m_subscribeReply;
    QNetworkReply *m_browseReply;
    QNetworkRequest m_GETrequest;
    QNetworkRequest m_subscribeRequest;
    QNetworkRequest m_browseRequest;
    QList<qint64> bytesReceived;
    QHostInfo server;
    QUrl m_remoteUrl;
    QList<QUrl> m_ownUrls;
    QUrl m_GETUrl;
    QUrl m_subscribeUrl;
    QUrl m_actionUrl;
    QFile *m_file;
    QByteArray *m_xmlByteArray;
    QByteArray m_answerFromServer;
    QList<QMap<QString, QString> > m_foundContent;
};

#endif // HTTPMANAGER_H
