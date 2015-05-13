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

    int init(QUrl descriptionUrl, QString eventSubUrl, QString controlUrl, QString serviceType);

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

    static const int tcpConnectTimeout = 5000;
    static const int firstByteReceivedTimeout = 5000;

    static const int defaultPort = 49160;

    QUrl remoteUrl() const;
    void setRemoteUrl(const QUrl &url);

    QHostInfo getServer() const;
    void setServer(const QHostInfo &value);

    QString servicetype() const;
    void setServicetype(const QString &servicetype);

    QList<QMap<QString, QString> > foundContent() const;

    QList<QUrl> ownUrls() const;
    void setOwnUrls(const QList<QUrl> &ownUrls);

    QList<QPair<QString, QString> > containerIDs() const;

    bool startTCPConnection();
    QList<QPair<QString, QString> > handleContent(QString t);
    QList<QPair<QString, QString> > sendRequest(QString objectID);
    QList<QPair<QString, QString> > read();
    int setupTCPSocketAndSend(QString objectID, int counter);

    int expectedLength() const;
    void setExpectedLength(int expectedLength);

public slots:
    int startGet();
    int GETreadyRead(QByteArray content);
    int subscribe();
    void disconnectionHandling();
    int printResults();
    void readSID();

signals:
    void TCPConnected(bool success);
    void TCPDisconnected();
    void connectTCP();
    void startDownload();
    void foundContainer();

private:
    QString m_servicetype;
    DownloadThreadStatus tStatus;
    Parser *m_parser;
    QTcpSocket *m_socket;
    QNetworkReply *m_GETReply;
    QNetworkReply *m_subscribeReply;
    QNetworkReply *m_browseReply;
    QNetworkRequest m_GETrequest;
    QNetworkRequest m_subscribeRequest;
    QNetworkRequest m_browseRequest;
    QList<qint64> bytesReceived;
    QHostInfo m_server;
    QUrl m_remoteUrl;
    QList<QUrl> m_ownUrls;
    QUrl m_GETUrl;
    QUrl m_subscribeUrl;
    QUrl m_actionUrl;
    QFile *m_file;
    QByteArray *m_xmlByteArray;
    QByteArray m_answerFromServer;
    QList<QMap<QString, QString> > m_foundContent;
    QList<QPair<QString, QString> > m_containerIDs;
    QList<QMap<QString, QString> > m_totalTableOfContents;
    QByteArray m_soapData;
    int m_expectedLength;
};

#endif // HTTPMANAGER_H
