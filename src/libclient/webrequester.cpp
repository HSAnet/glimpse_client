#include "webrequester.h"
#include "client.h"
#include "settings.h"
#include "log/logger.h"
#include "network/requests/loginrequest.h"

#include <QTimer>
#include <QPointer>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrlQuery>
#include <QJsonDocument>
#include <QMetaClassInfo>
#include <QDebug>
#include <QFile>
#include <QSslKey>

LOGGER(WebRequester);

class WebRequester::Private : public QObject
{
    Q_OBJECT

public:
    Private(WebRequester *q)
    : q(q)
    , status(WebRequester::Unknown)
    {
        connect(&timer, SIGNAL(timeout()), this, SLOT(timeout()));

        timer.setSingleShot(true);
        timer.setInterval(5*60*1000); // five minutes for sending and getting a reply
    }

    WebRequester *q;

    // Properties
    WebRequester::Status status;
    QUrl url;
    QPointer<Request> request;
    QPointer<Response> response;
    QString errorString;

    QJsonObject jsonData;

    QTimer timer;
    QPointer<QNetworkReply> currentReply;

    // Functions
    void setStatus(WebRequester::Status status);

public slots:
    void requestFinished();
    void timeout();
};

void WebRequester::Private::setStatus(WebRequester::Status status)
{
    if (this->status != status)
    {
        this->status = status;
        emit q->statusChanged(status);

        switch (status)
        {
        case Running:
            emit q->started();
            break;

        case Finished:
            emit q->finished();
            break;

        case Error:
            emit q->error();
            break;

        default:
            break;
        }
    }
}

void WebRequester::Private::requestFinished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    QNetworkReply::NetworkError networkError = reply->error();

    if (networkError == QNetworkReply::NoError)
    {
        QJsonParseError jsonError;
        QByteArray data = reply->readAll();

        QJsonDocument document = QJsonDocument::fromJson(data, &jsonError);

        if (jsonError.error == QJsonParseError::NoError)
        {
            QJsonObject root = document.object();

            QString replyError = root.value("error").toString();

            if (replyError.isEmpty())
            {
                jsonData = root;

                if (!response.isNull())
                {
                    if (response->fillFromVariant(root.toVariantMap()))
                    {
                        response->finished();
                        setStatus(WebRequester::Finished);
                    }
                    else
                    {
                        setStatus(WebRequester::Error);
                    }
                }
                else
                {
                    LOG_WARNING("No response object set");
                    setStatus(WebRequester::Finished);
                }
            }
            else
            {
                errorString = replyError;
                LOG_WARNING(QString("Error from server: %1").arg(errorString));
                setStatus(WebRequester::Error);
            }
        }
        else
        {
            if (!data.isEmpty())
            {
                errorString = jsonError.errorString();
                LOG_WARNING(QString("JsonParseError: %1 (%2)").arg(errorString).arg(QString(data)));
                setStatus(WebRequester::Error);
            }
            else // empty data is okay if no network error occured (= 2xx response code)
            {
                if (!response.isNull())
                {
                    response->finished();
                }

                setStatus(WebRequester::Finished);
            }
        }
    }
    else
    {
        if (!timer.isActive())
        {
            errorString = tr("Operation timed out");
        }
        else
        {
            QVariant statusCode = reply->attribute( QNetworkRequest::HttpStatusCodeAttribute );
            if (statusCode.isValid() && statusCode.toInt() == 401)
            {
                errorString = tr("Email or Password wrong");
            }
            else
            {
                errorString = reply->errorString();
            }

            QJsonParseError jsonError;
            QByteArray data = reply->readAll();

            QJsonDocument document = QJsonDocument::fromJson(data, &jsonError);

            if (jsonError.error == QJsonParseError::NoError)
            {
                QJsonObject root = document.object();

                QString replyError =
                    root.value("error_message").toString(); // in some django replys its error, in some its error_message

                if (!replyError.isEmpty())
                {
                    LOG_WARNING(QString("Error message: %1").arg(replyError));
                    errorString = replyError;
                }

                replyError = root.value("error").toString();

                if (!replyError.isEmpty())
                {
                    LOG_WARNING(QString("Error message: %1").arg(replyError));
                    errorString = replyError;
                }
            }
        }
        setStatus(WebRequester::Error);
    }

    // Always stop the timeout timer
    timer.stop();

    reply->deleteLater();
}

void WebRequester::Private::timeout()
{
    // Abort current reply
    if (currentReply)
    {
        currentReply->abort();
    }
}

WebRequester::WebRequester(QObject *parent)
: QObject(parent)
, d(new Private(this))
{
}

WebRequester::~WebRequester()
{
    delete d;
}

WebRequester::Status WebRequester::status() const
{
    return d->status;
}

void WebRequester::setTimeout(int ms)
{
    if (d->timer.interval() != ms)
    {
        d->timer.setInterval(ms);
        emit timeoutChanged(ms);
    }
}

int WebRequester::timeout() const
{
    return d->timer.interval();
}

void WebRequester::setUrl(const QUrl &url)
{
    if (d->url != url)
    {
        d->url = url;
        emit urlChanged(url);
    }
}

QUrl WebRequester::url() const
{
    return d->url;
}

void WebRequester::setRequest(Request *request)
{
    if (d->request != request)
    {
        d->request = request;
        emit requestChanged(request);
    }
}

Request *WebRequester::request() const
{
    return d->request;
}

void WebRequester::setResponse(Response *response)
{
    if (d->response != response)
    {
        d->response = response;
        emit responseChanged(response);
    }
}

Response *WebRequester::response() const
{
    return d->response;
}

bool WebRequester::isRunning() const
{
    return (d->status == Running);
}

QString WebRequester::errorString() const
{
    return d->errorString;
}

QVariant WebRequester::jsonDataQml() const
{
    return d->jsonData.toVariantMap();
}

QJsonObject WebRequester::jsonData() const
{
    return d->jsonData;
}

void WebRequester::start()
{
    if (!d->url.isValid())
    {
        d->errorString = tr("Invalid url: %1").arg(d->url.toString());
        d->setStatus(Error);
        LOG_ERROR(QString("Invalid url: %1").arg(d->url.toString()));
        return;
    }

    if (!d->request)
    {
        d->errorString = tr("No request to start");
        d->setStatus(Error);
        LOG_ERROR("No request to start");
        return;
    }

    // Get path
    QString path = d->request->path();

    if (path.isEmpty())
    {
        d->errorString = tr("No path found for request");
        d->setStatus(Error);
        LOG_ERROR("No path found for request");
        return;
    }

    const QMetaObject *metaObject = d->request->metaObject();
    int methodIdx = metaObject->indexOfClassInfo("http_request_method");

    if (methodIdx == -1)
    {
        d->errorString = tr("No http_request_method specified");
        d->setStatus(Error);
        LOG_ERROR("No http_request_method specified");
        return;
    }

    QString httpMethod = metaObject->classInfo(methodIdx).value();

    d->setStatus(Running);

    const Settings *settings = Client::instance()->settings();

    // Fill remaining request data
    d->request->setDeviceId(settings->deviceId());
    d->request->setSessionId(settings->apiKey());


    QVariantMap data = d->request->toVariant().toMap();

    QUrl url = d->url;
    url.setPath(path);

    QNetworkRequest request;
    QNetworkReply *reply;

    Request::AuthenticationMethod authenticationMethod = d->request->authenticationMethod();

    if (authenticationMethod == Request::Basic)
    {
        // this has to be a login request
        QPointer<LoginRequest> loginRequest = qobject_cast<LoginRequest*>(d->request);

        if (loginRequest.isNull())
        {
            d->errorString = tr("Wrong Request used for Webrequester");
            d->setStatus(Error);
            LOG_ERROR("Wrong Request used for Webrequester");
            return;
        }

        url.setUserName(settings->hashedUserId());
        url.setPassword(loginRequest->password());
    }
    else if (authenticationMethod == Request::ApiKey)
    {
        request.setRawHeader("Authorization", QString("ApiKey %1:%2").arg(settings->hashedUserId()).arg(
                                 settings->apiKey()).toUtf8());
    }
    else if (authenticationMethod == Request::None)
    {

    }

    // load the certificate if needed
    if (!settings->clientCert().isEmpty() && !settings->caCert().isEmpty() &&
            !settings->privateKey().isEmpty())
    {
        QSslConfiguration configuration;
        QFile certFile(settings->clientCert()); //"/home/mike/tmp/certs/Client-SSB.crt");
        certFile.open(QFile::ReadOnly);
        QSslCertificate cert(&certFile);
        certFile.close();
        configuration.setLocalCertificate(cert);

        QList<QSslCertificate> certs;
        certFile.setFileName(settings->caCert()); //"/home/mike/tmp/certs/root-ca.crt");
        certFile.open(QFile::ReadOnly);
        QSslCertificate caCert(&certFile);
        certFile.close();
        certs.append(caCert);

        configuration.setCaCertificates(certs);

        QFile keyFile(settings->privateKey()); // "/home/mike/tmp/certs/Client-SSB-plaintext.key");
        keyFile.open(QFile::ReadOnly);
        QSslKey key(&keyFile, QSsl::Rsa, QSsl::Pem, QSsl::PrivateKey);
        configuration.setPrivateKey(key);
        keyFile.close();

        request.setSslConfiguration(configuration);
    }

    if (httpMethod == "get")
    {
        QUrlQuery query(url);

        QMapIterator<QString, QVariant> iter(data);

        while (iter.hasNext())
        {
            iter.next();

            query.addQueryItem(iter.key(), iter.value().toString());
        }

        url.setQuery(query);

        request.setUrl(url);
        reply = Client::instance()->networkAccessManager()->get(request);
    }
    else if (httpMethod == "post")
    {
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-mplane+json");
        request.setUrl(url);

        // compress data, remove the first four bytes (which is the array length which does not belong there), convert to base64
        reply = Client::instance()->networkAccessManager()->post(request, QJsonDocument::fromVariant(data).toJson());
    }
    else
    {
        d->errorString = tr("http_request_method unknown");
        d->setStatus(Error);
        LOG_ERROR("http_request_method unknown");
        return;
    }

    connect(reply, SIGNAL(sslErrors(QList<QSslError>)), this, SLOT(sslErrors(QList<QSslError>)));
    connect(reply, SIGNAL(finished()), d, SLOT(requestFinished()));

    // Wait for timeout
    d->currentReply = reply;
    d->timer.start();
}

void WebRequester::sslErrors(QList<QSslError> errors)
{
    foreach (QSslError error, errors)
    {
        LOG_ERROR(QString("SSL-Error: %1").arg(error.errorString()));

    }
}

#include "webrequester.moc"
