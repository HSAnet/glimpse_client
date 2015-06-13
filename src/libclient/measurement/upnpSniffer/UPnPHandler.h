/*
 * In this class all major parts of connecting with a UPnP-Server:
 * getting a description of the server, subscribing and finally
 * browsing its contents are realized.
 *
 * get: startGET()
 * subscibe: subscribe()
 * recursive browsing: setupTCPSocketAndSend()
 */

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
    UPnPHandler();
    ~UPnPHandler();

    /* Static Values */
    static const int tcpConnectTimeout = 5000;
    static const int firstByteReceivedTimeout = 5000;
    static const int defaultPort = 33333;

    int init(QUrl descriptionUrl, QString eventSubUrl, QString controlUrl, QString serviceType);
    int cleanup();
    bool startTCPConnection();

    /* All parts of the recursion */
    int setupTCPSocketAndSend(QString objectID, int counter);
    QList<QPair<QString, QString> > handleContent(QString t);
    QList<QPair<QString, QString> > sendRequest(QString objectID);
    QList<QPair<QString, QString> > read();

    /* Setters */
    void setFile(const QString filename);
    void setSubscribeUrl(const QUrl &subscribeUrl);
    void setActionUrl(const QUrl &actionUrl);
    void setRemoteUrl(const QUrl &url);
    void setServicetype(const QString &servicetype);
    void setOwnUrls(const QList<QUrl> &ownUrls);

    /* Getters */
    QUrl subscribeUrl() const;
    QUrl actionUrl() const;
    QUrl remoteUrl() const;
    QString servicetype() const;
    QList<QMap<QString, QString> > foundContent() const;
    QList<QMap<QString, QString> > totalTableOfContents() const;


public slots:
    int startGet();
    int getReadyRead(QByteArray content);
    int subscribe();
    int printResults();
    void readSID();

signals:
    void connectTCP();
    void foundContainer();

private:
    QString m_servicetype;
    Parser *m_parser;
    QTcpSocket *m_socket;
    QNetworkReply *m_GETReply;
    QNetworkReply *m_subscribeReply;
    QNetworkReply *m_browseReply;
    QNetworkRequest m_GETrequest;
    QNetworkRequest m_subscribeRequest;
    QNetworkRequest m_browseRequest;
    QList<qint64> bytesReceived;
    QUrl m_remoteUrl;
    QList<QUrl> m_ownUrls;
    QUrl m_GETUrl;
    QUrl m_subscribeUrl;
    QUrl m_actionUrl;
    QFile *m_file;
    QByteArray m_answerFromServer;
    QList<QMap<QString, QString> > m_foundContent;
    QList<QMap<QString, QString> > m_totalTableOfContents;
    QByteArray m_soapData;
    int m_expectedLength;
};

#endif // HTTPMANAGER_H
