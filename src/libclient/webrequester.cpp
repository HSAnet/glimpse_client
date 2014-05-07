#include "webrequester.h"
#include "client.h"
#include "settings.h"
#include "log/logger.h"

#include <QTimer>
#include <QPointer>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QMetaClassInfo>
#include <QDebug>

LOGGER(WebRequester);

class WebRequester::Private : public QObject
{
    Q_OBJECT

public:
    Private(WebRequester* q)
    : q(q)
    , status(WebRequester::Unknown)
    {
        connect(&timer, SIGNAL(timeout()), this, SLOT(timeout()));

        timer.setSingleShot(true);
        timer.setInterval(5000);
    }

    WebRequester* q;

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
    if ( this->status != status )
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
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());

    if (reply->error() == QNetworkReply::NoError)
    {
        QJsonParseError error;
        QJsonDocument document = QJsonDocument::fromJson(reply->readAll(), &error);

        if ( error.error == QJsonParseError::NoError )
        {
            QJsonObject root = document.object();

            QString replyError = root.value("error").toString();
            if ( replyError.isEmpty() )
            {
                jsonData = root;

                if ( !response.isNull() )
                {
                    if ( response->fillFromVariant( root.toVariantMap() ) )
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
                setStatus(WebRequester::Error);
            }
        }
        else
        {
            errorString = error.errorString();
            setStatus(WebRequester::Error);
        }
    }
    else
    {
        if (!timer.isActive())
        {
            errorString = "Operation timed out";
        }
        else
        {
            errorString = reply->errorString();
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
        currentReply->abort();
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
    if ( d->timer.interval() != ms )
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
    if ( d->url != url )
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
    if ( d->request != request )
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
    if ( d->response != response )
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
    /*
    if ( !d->url.isValid() ) {
        d->errorString = tr("Invalid url: %1").arg(d->url.toString());
        d->setStatus(Error);
        LOG_ERROR(QString("Invalid url: %1").arg(d->url.toString()));
        return;
    }
    */

    if ( !d->request )
    {
        d->errorString = tr("No request to start");
        d->setStatus(Error);
        LOG_ERROR("No request to start");
        return;
    }

    int pathIdx = d->request->metaObject()->indexOfClassInfo("path");
    if ( pathIdx == -1 )
    {
        d->errorString = tr("No path found for request");
        d->setStatus(Error);
        LOG_ERROR("No path found for request");
        return;
    }

    d->setStatus(Running);

    // Fill remaining request data
    d->request->setDeviceId(Client::instance()->settings()->deviceId());
    d->request->setSessionId(Client::instance()->settings()->sessionId());

    QMetaClassInfo classInfo = d->request->metaObject()->classInfo(pathIdx);

    QByteArray data = QJsonDocument::fromVariant(d->request->toVariant()).toJson();

    // TODO: Use internal url here
    QUrl url = QString("https://%1").arg(Client::instance()->settings()->config()->controllerAddress());
    url.setPath(classInfo.value());

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "text/json");

    QNetworkReply* reply = Client::instance()->networkAccessManager()->post(request, data);
    reply->ignoreSslErrors();
    connect(reply, SIGNAL(finished()), d, SLOT(requestFinished()));

    // Wait for timeout
    d->currentReply = reply;
    d->timer.start();
}

#include "webrequester.moc"
