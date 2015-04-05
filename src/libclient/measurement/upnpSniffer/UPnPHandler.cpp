#include "UPnPHandler.h"
#include <QHttpPart>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkConfiguration>
//#include <QXmlInputSource>
#include <QCoreApplication>
#include <QNetworkInterface>

#include <QXmlStreamWriter>
#include <QThread>
#include <iterator>

UPnPHandler::UPnPHandler()
{
}


UPnPHandler::~UPnPHandler()
{

}

int UPnPHandler::init(QUrl remoteUrl, QString descriptionUrl, QString eventSubUrl, QString controlUrl, QString serviceType)
{
/*  old code:
    QUrl mediatombUrl = QUrl("http://172.16.172.1:49152/");
    QUrl myUrl = (QUrl("http://172.16.172.1:49152"));
    QString getPath = "/description.xml"; //descriptionUrl ... complete!
    QString eventPath = "/upnp/event/cds"; //eventSubUrl ... only path
    QString actionPath = "/upnp/control/cds"; //controlUrl ... only path */
    QList<QUrl> ownUrls;
    foreach (const QHostAddress &address, QNetworkInterface::allAddresses()) {
        if (address.protocol() == QAbstractSocket::IPv4Protocol && address != QHostAddress(QHostAddress::LocalHost))
        {
            QString s = QString("http://%1").arg(address.toString());
            ownUrls.append(QUrl(s));
        }
    }

    setRemoteUrl(remoteUrl);
    setOwnUrls(ownUrls); //TODO check if valid for all urls

    QUrl subscribeUrl, browseUrl;
    QUrl descUrl(descriptionUrl);
    browseUrl = subscribeUrl = m_remoteUrl;
    browseUrl.setPath(controlUrl);
    subscribeUrl.setPath(eventSubUrl);

    setGETUrl(descUrl);
    setActionUrl(browseUrl);
    setSubscribeUrl(subscribeUrl);
    setServicetype(serviceType);
    setFile("/home/simon/code/upnpSniffer/xmlInfoFromVlc.xml"); //TODO
    m_answerFromServer = "";

    m_parser = new Parser();
    setNetworkAccessManager(new QNetworkAccessManager());
    startGet();
    connect(m_GETReply, SIGNAL(readyRead()),this, SLOT(GETreadyRead()));
//    connect(m_GETReply, SIGNAL(finished()), this, SLOT(GETFinished())); //Note: finished() signal comes after readyRead()
    connect(m_parser, SIGNAL(xmlParsed()), this, SLOT(subscribe()));
    connect(this, SIGNAL(subscribed()), this, SLOT(startAction()));
    connect(this, SIGNAL(readyToParse()), m_parser, SLOT(parseAnswer()));
    connect(m_parser, SIGNAL(contentFound()), this, SLOT(handleContent()));
    return 0;
}

QNetworkAccessManager * UPnPHandler::networkAccessManager() const
{
    return m_networkAccessManager;
}

void UPnPHandler::setNetworkAccessManager(QNetworkAccessManager *networkAccessManager)
{
    m_networkAccessManager = networkAccessManager;
}

void UPnPHandler::GETreadyRead()
{
    if(m_GETReply->error() != QNetworkReply::NoError)
    {
        qDebug() << "Error occured while get-request:" , m_GETReply->errorString();
        return;
    }
    QByteArray content = m_GETReply->readAll();
    m_xmlByteArray->append(content);
    qDebug() << "Reply:" + *m_xmlByteArray + "EOF";
    /* comes in multiple packets */
    m_parser->parseXML(*m_xmlByteArray);
}

void UPnPHandler::subscribe()
{
    /* Now subscribe */
    m_subscribeRequest.setUrl(m_subscribeUrl);
    QByteArray val = ("</>");
//    val.insert(1, m_myUrl.toString());
//    val.insert(1,"http://192.168.2.104:49153");
    /* trying all localhost addresses */
    foreach(QUrl u, m_ownUrls)
    {
        u.setPort(49153); //That is the vlc port, using it since it works in vlc
        val.insert(1, u.toString());
        m_subscribeRequest.setRawHeader(QByteArray("CALLBACK"), val);
        m_subscribeRequest.setRawHeader(QByteArray("NT"), ("upnp:event"));

        m_subscribeReply = m_networkAccessManager->sendCustomRequest(m_subscribeRequest, "SUBSCRIBE");
        qDebug() << m_subscribeReply->readAll(); //TODO probably the reply is unneccesary -> delete
    }
    emit subscribed();
}

//void UPnPHandler::browse()
//{

//    QByteArray deviceFullName = "\"urn:schemas-upnp-org:service:ContentDirectory:1";
//    QByteArray key = "SOAPACTION";
//    QByteArray action = "#Browse\""; // quotes needed!
//    deviceFullName.append(action);
//    m_browseRequest.setUrl(m_actionUrl);
//    m_browseRequest.setHeader(QNetworkRequest::ContentTypeHeader, "text/xml; charset=\"utf-8\"");
//    m_browseRequest.setRawHeader(key, deviceFullName);
//    m_browseRequest.setRawHeader(QByteArray("USER-AGENT"), QByteArray("Linux/3.13.0-24-generic, UPnP/1.0, Portable SDK for UPnP devices/1.6.17"));
//    QNetworkConfiguration conf = m_networkAccessManager->configuration();
//    m_browseRequest.setRawHeader("Accept-Language", QByteArray());
//    //    m_browseReply = m_networkAccessManager->post(m_browseRequest, xmlRaw);
//}

void UPnPHandler::sendRequest()
{
    //we can only download, if this thread sucessfully established the
    //TCP connection (all threads will receive the signal)
    if(m_socket->state() != QAbstractSocket::ConnectedState)
    {
        tStatus = FinishedError;
        //emit firstByteReceived(false);
        //TODO error
        return;
    }

    if (!m_file->open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "Opening XML File Problem";
        exit(-1);
    }
    QByteArray xmlRaw = m_file->readAll();
    //different values for the object-IDs need to be inserted. starting with 0;
    //Parsing the answer and then again send request to the corresponding object-IDs -> new loop or slots TODO
    QString data(xmlRaw);
    int dataLength = data.length();
    QString url = m_remoteUrl.toString();
    url.remove("http://");

    QString header = QString("POST %1 HTTP/1.1\r\n"
                              "HOST: %2\r\n"
                              "CONTENT-LENGTH: %3\r\n"
                              "CONTENT-TYPE: text/xml; charset=\"utf-8\"\r\n"
                              "SOAPACTION: \"%4\#Browse\"\r\n"
                              "USER-AGENT: Linux/3.13.0-24-generic, UPnP/1.0, Portable SDK for UPnP devices/1.6.17\r\n\r\n").arg(m_actionUrl.path())
                                                                                                                            .arg(url)
                                                                                                                            .arg(dataLength)
                                                                                                                            .arg(m_servicetype);
    int bytesWritten = 0;

    //send the SOAP request -> HTTP POST and then the xml
    while(bytesWritten < header.length())
    {
        bytesWritten += m_socket->write(header.mid(bytesWritten).toLatin1());

        //on error
        if(bytesWritten < 0)
        {
            m_socket->close();
            tStatus = FinishedError;
            emit firstByteReceived(false);
            return;
        }
    }
    bytesWritten = 0;
    while(bytesWritten < data.length())
    {
        bytesWritten += m_socket->write(data.toLatin1());

        //on error
        if(bytesWritten < 0)
        {
            m_socket->close();
            tStatus = FinishedError;
            emit firstByteReceived(false);
            return;
        }
    }
    tStatus = AwaitingFirstByte;

    //do a blocking read for the first bytes or time-out if nothing is arriving
    if (m_socket->waitForReadyRead(firstByteReceivedTimeout))
    {
        tStatus = DownloadInProgress;
        //connect readyRead of the socket with read() for further reads
        connect(m_socket, &QTcpSocket::readyRead, this, &UPnPHandler::read);
        //call read
        read();
        emit firstByteReceived(true);
    }
    else
    {
        tStatus = FinishedError;
        QString s = m_socket->errorString();
        m_socket->close();
        emit firstByteReceived(false);
    }
}

void UPnPHandler::startAction()
{
    QHostInfo::lookupHost(m_actionUrl.host(), this, SLOT(setupTCPSocket(QHostInfo)));
}

void UPnPHandler::setupTCPSocket(const QHostInfo &server)
{
    //check if the name resolution was actually successful
    QString s = server.errorString();
    if (server.error() != QHostInfo::NoError)
    {
        //emit error("Name resolution failed");
        return;
    }
    setServer(server);
    //this signal tells a worker thread to initiate the TCP 3-way handshake
    connect(this, SIGNAL(connectTCP()), this, SLOT(startTCPConnection()));

    //this signal is for the this thread of execution to track the TCP connection state
    //of the worker threads
    connect(this, SIGNAL(TCPConnected(bool)), this, SLOT(TCPConnectionTracking(bool)));

    //this signal tells the threads to start the actual download
    connect(this, SIGNAL(startDownload()), this, SLOT(sendRequest()));

    connect(this, SIGNAL(firstByteReceived(bool)), this, SLOT(downloadStartedTracking(bool)));

    connect(this, SIGNAL(TCPDisconnected()), this, SLOT(tidyUp())); //TODO errorhandling like winter

    emit connectTCP();
}

void UPnPHandler::read()
{
    bytesReceived << m_socket->bytesAvailable();
    QByteArray ba = m_socket->readAll();
    m_answerFromServer.append(ba);
    m_parser->setRawData(m_answerFromServer);
    emit readyToParse();
}

void UPnPHandler::disconnectionHandling()
{
    // handling premature TCP disconnects
    if(tStatus == DownloadInProgress)
    {
        tStatus = FinishedSuccess;
        emit TCPDisconnected();
    }
}

void UPnPHandler::startTCPConnection()
{
    //shouldn't happen, check anyway
    if (server.addresses().isEmpty() || (!m_actionUrl.isValid()))
    {
        //invoke the connection tracking code
        tStatus = FinishedError;
        return;
    }
    m_socket = new QTcpSocket();

    //so let's connect now (if no port as part of the URL use 80 as default)
    int p = m_actionUrl.port();
    m_socket->connectToHost(server.addresses().first(), p);

    tStatus = ConnectingTCP;

    //wait for up to 5 seconds for a successful connection
    if (m_socket->waitForConnected(tcpConnectTimeout))
    {
        //for the successfully connected sockets, we should track the disconnection
        connect(m_socket, SIGNAL(disconnected()), this, SLOT(disconnectionHandling()));
        tStatus = ConnectedTCP;
        emit TCPConnected(true);
    }
    else
    {
        //something went wrong
        QString s = m_socket->errorString();
        tStatus = FinishedError;
        emit TCPConnected(false);
        m_socket->close();
    }
}

void UPnPHandler::TCPConnectionTracking(bool success)
{
    if (success)
    {
        emit startDownload();
    }
}

void UPnPHandler::downloadStartedTracking(bool success)
{
    if(success)
    {
        qDebug() << "SUCCESS download started";
    }
//TODO
}

void UPnPHandler::tidyUp()
{
    m_file->close();
}

void UPnPHandler::handleContent()
{
    m_foundContent = m_parser->getFoundContent();
    //TODO...
    //example:
    if(m_foundContent.length() != 0)
    {
        qDebug() << "lalala" + m_foundContent.at(1).value("title");
    }
    emit handlingDone();

    QCoreApplication::exit(0);
}
QList<QUrl> UPnPHandler::ownUrls() const
{
    return m_ownUrls;
}

void UPnPHandler::setOwnUrls(const QList<QUrl> &ownUrls)
{
    m_ownUrls = ownUrls;
}

QList<QMap<QString, QString> > UPnPHandler::foundContent() const
{
    return m_foundContent;
}

QString UPnPHandler::servicetype() const
{
    return m_servicetype;
}

void UPnPHandler::setServicetype(const QString &servicetype)
{
    m_servicetype = servicetype;
}

void UPnPHandler::startGet()
{
    m_GETrequest.setUrl(m_GETUrl);
    m_GETrequest.setRawHeader(QByteArray("USER-AGENT"), QByteArray("Linux/3.13.0-24-generic, UPnP/1.0, Portable SDK for UPnP devices/1.6.17"));

    qDebug() << "Sending get request ...";
    m_xmlByteArray = new QByteArray();
    m_GETReply = m_networkAccessManager->get(m_GETrequest);
}

QHostInfo UPnPHandler::getServer() const
{
    return server;
}

void UPnPHandler::setServer(const QHostInfo &value)
{
    server = value;
}

QUrl UPnPHandler::remoteUrl() const
{
    return m_remoteUrl;
}

void UPnPHandler::setRemoteUrl(const QUrl &url)
{
    m_remoteUrl = url;
}


QUrl UPnPHandler::actionUrl() const
{
    return m_actionUrl;
}

void UPnPHandler::setActionUrl(const QUrl &browseUrl)
{
    m_actionUrl = browseUrl;
}


QUrl UPnPHandler::subscribeUrl() const
{
    return m_subscribeUrl;
}

void UPnPHandler::setSubscribeUrl(const QUrl &subscribeUrl)
{
    m_subscribeUrl = subscribeUrl;
}


QFile *UPnPHandler::file() const
{
    return m_file;
}

void UPnPHandler::setFile(const QString filename)
{
    m_file = new QFile(filename);
}

QNetworkRequest UPnPHandler::GETrequest() const
{
    return m_GETrequest;
}

void UPnPHandler::setGETrequest(const QNetworkRequest &GETrequest)
{
    m_GETrequest = GETrequest;
}

QUrl UPnPHandler::GETUrl() const
{
    return m_GETUrl;
}

void UPnPHandler::setGETUrl(const QUrl &GETUrl)
{
    m_GETUrl = GETUrl;
}




