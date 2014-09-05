#include "webrequester.h"
#include "client.h"
#include "settings.h"
#include "log/logger.h"

#include <QTimer>
#include <QPointer>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrlQuery>
#include <QJsonDocument>
#include <QMetaClassInfo>
#include <QDebug>

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
        timer.setInterval(5000);
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
        }

        LOG_WARNING(QString("Network error: %1").arg(errorString));

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

    int methodIdx = d->request->metaObject()->indexOfClassInfo("http_request_method");

    if (methodIdx == -1)
    {
        d->errorString = tr("No http_request_method specified");
        d->setStatus(Error);
        LOG_ERROR("No http_request_method specified");
        return;
    }

    QString httpMethod = d->request->metaObject()->classInfo(methodIdx).value();

    int authenticationIdx = d->request->metaObject()->indexOfClassInfo("authentication_method");
    QString authentication = "none";

    if (authenticationIdx != -1)
    {
        authentication = d->request->metaObject()->classInfo(authenticationIdx).value();
    }

    d->setStatus(Running);

    // Fill remaining request data
    d->request->setDeviceId(Client::instance()->settings()->deviceId());
    d->request->setSessionId(Client::instance()->settings()->apiKey());


    QVariantMap data = d->request->toVariant().toMap();

    QUrl url = d->url;
    url.setPath(path);

    QNetworkRequest request;
    QNetworkReply *reply;

    if (authentication == "basic")
    {
        url.setUserName(Client::instance()->settings()->userId());
        url.setPassword(
            Client::instance()->settings()->password()); // TODO change this to a temporary variable after login-form
        // which is not saved into the settings
    }
    else if (authentication == "apikey")
    {
        request.setRawHeader("Authorization",
                             QString("ApiKey %1:%2").arg(Client::instance()->settings()->userId().left(30)).arg(
                                 Client::instance()->settings()->apiKey()).toUtf8());
    }
    else if (authentication == "none")
    {

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
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
        request.setUrl(url);
        reply = Client::instance()->networkAccessManager()->post(request, QJsonDocument::fromVariant(data).toJson());
    }
    else
    {
        d->errorString = tr("http_request_method unknown");
        d->setStatus(Error);
        LOG_ERROR("http_request_method unknown");
        return;
    }

    connect(reply, SIGNAL(finished()), d, SLOT(requestFinished()));

    // Wait for timeout
    d->currentReply = reply;
    d->timer.start();
}

#include "webrequester.moc"
