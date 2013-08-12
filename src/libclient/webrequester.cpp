#include "webrequester.h"
#include "client.h"

#include <QPointer>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QJsonDocument>

namespace {
    const QUrl masterUrl = QUrl("https://141.82.49.82:5105");
}

class WebRequester::Private : public QObject
{
    Q_OBJECT

public:
    Private(WebRequester* q)
    : q(q)
    , status(WebRequester::Unknown)
    {
    }

    WebRequester* q;

    // Properties
    WebRequester::Status status;
    QPointer<Request> request;
    QPointer<Response> response;
    QString errorString;

    QJsonObject jsonData;

    // Functions
    void setStatus(WebRequester::Status status);

public slots:
    void requestFinished();
};

void WebRequester::Private::setStatus(WebRequester::Status status)
{
    if ( this->status != status ) {
        this->status = status;
        emit q->statusChanged(status);

        switch(status) {
        case Finished: emit q->finished(); break;
        case Error: emit q->error(); break;
        }
    }
}

void WebRequester::Private::requestFinished()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());

    if (reply->error() == QNetworkReply::NoError) {
        QJsonParseError error;
        QJsonDocument document = QJsonDocument::fromJson(reply->readAll(), &error);

        if ( error.error == QJsonParseError::NoError ) {
            QJsonObject root = document.object();

            QString replyError = root.value("error").toString();
            if ( replyError.isEmpty() ) {
                jsonData = root;

                if ( !response.isNull() ) {
                    if ( response->fillFromVariant( root.toVariantMap() ) )
                        setStatus(WebRequester::Finished);
                    else
                        setStatus(WebRequester::Error);
                } else {
                    qWarning() << "No response object set";
                    setStatus(WebRequester::Finished);
                }
            } else {
                errorString = replyError;
                setStatus(WebRequester::Error);
            }
        } else {
            errorString = error.errorString();
            setStatus(WebRequester::Error);
        }
    } else {
        errorString = reply->errorString();
        setStatus(WebRequester::Error);
    }

    reply->deleteLater();
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

void WebRequester::setRequest(Request *request)
{
    if ( d->request != request ) {
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
    if ( d->response != response ) {
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
    if ( !d->request ) {
        d->errorString = tr("No request to start");
        d->setStatus(Error);
        qDebug() << "No request to start";
        return;
    }

    int pathIdx = d->request->metaObject()->indexOfClassInfo("path");
    if ( pathIdx == -1 ) {
        d->errorString = tr("No path found for request");
        d->setStatus(Error);
        qDebug() << "No path found for request";
        return;
    }

    d->setStatus(Running);

    // Fill remaining request data
    d->request->setDeviceId(Client::instance()->settings()->deviceId());
    d->request->setSessionId(Client::instance()->settings()->sessionId());

    QMetaClassInfo classInfo = d->request->metaObject()->classInfo(pathIdx);

    QByteArray data = QJsonDocument::fromVariant(d->request->toVariant()).toJson();

    QUrl url = masterUrl;
    url.setPath(classInfo.value());

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "text/json");

    QNetworkReply* reply = Client::instance()->networkAccessManager()->post(request, data);
    reply->ignoreSslErrors();
    connect(reply, SIGNAL(finished()), d, SLOT(requestFinished()));
}

#include "webrequester.moc"
