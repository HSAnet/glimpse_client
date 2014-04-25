#include "httpdownload.h"
#include "../../log/logger.h"

#include <numeric>
#include <QtCore/QtMath>

LOGGER(HTTPDownload);


HTTPDownload::HTTPDownload(QObject *parent)
: Measurement(parent)
, m_lasttime(0)
, currentStatus(HTTPDownload::Unknown)
, m_nam(NULL)
, m_reply(NULL)
{
}

HTTPDownload::~HTTPDownload()
{
}

Measurement::Status HTTPDownload::status() const
{
    return currentStatus;
}

bool HTTPDownload::prepare(NetworkManager *networkManager, const MeasurementDefinitionPtr &measurementDefinition)
{
    Q_UNUSED(networkManager)
    definition = measurementDefinition.dynamicCast<HTTPDownloadDefinition>();
    if ( definition.isNull() )
    {
        LOG_WARNING("Definition is empty");
        return false;
    }

    currentStatus = HTTPDownload::Unknown;

    // get network access manager
    m_nam = new QNetworkAccessManager();

    // prepare request
    m_request = QNetworkRequest(definition->url);

    return true;
}

bool HTTPDownload::start()
{
    // start timer
    m_timer.start();

    // start
    m_reply = m_nam->get(m_request);
    setStartDateTime(QDateTime::currentDateTime());
    setStatus(HTTPDownload::Running);

    QObject::connect(m_reply, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(downloadProgress(qint64,qint64)));
    QObject::connect(m_reply, SIGNAL(finished()), this, SLOT(requestFinished()));

    return true;
}

bool HTTPDownload::stop()
{
    return true;
}

ResultPtr HTTPDownload::result() const
{
    QVariantList res;
    foreach(qreal val, m_downloadSpeeds)
    {
        res<<QString::number(val, 'f');
    }

    return ResultPtr(new Result(startDateTime(), QDateTime::currentDateTime(), res, QVariant()));
}

void HTTPDownload::setStatus(Status status)
{
    if (currentStatus != status)
    {
        currentStatus = status;
        emit statusChanged(status);
    }
}

void HTTPDownload::requestFinished()
{
    QVariant statusCode = m_reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);

    if (m_reply->error() == QNetworkReply::NoError)
    {
        // calculate download speed for n slices
        int slices = 5; // TODO 5 slices, as parameter?
        int slice_size = ceil((double)m_times.size() / slices); // round up to get the correct number of slices

        // counters
        qint64 bytes = 0;
        qint64 time = 0;

        // calculate slices
        for (int i=0; i<m_times.size(); i++)
        {
            bytes += m_bytesReceived.at(i);
            time += m_times.at(i);

            // calculate a slice if enough samples are added up or if this is the last iteration
            if (((i+1) % slice_size) == 0 || i == m_times.size() - 1)
            {
                qreal speed = (bytes / 1024.0) / (((time / 1000.0) / 1000) / 1000);
                m_downloadSpeeds<<speed;

                // reset counters
                bytes = 0;
                time = 0;
            }
        }

        // calculate mean speed
        qreal sum = std::accumulate(m_downloadSpeeds.begin(), m_downloadSpeeds.end(), 0.0);
        qreal mean = sum / m_downloadSpeeds.size();
        LOG_INFO(QString("Speed (mean): %1 KByte/s").arg(mean, 0, 'f', 0));
    }
    else
    {
        // handle errors here
    }

    setStatus(HTTPDownload::Finished);
    emit finished();
}

void HTTPDownload::downloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    Q_UNUSED(bytesReceived);
    Q_UNUSED(bytesTotal);

    qint64 time = m_timer.nsecsElapsed();

    m_bytesReceived<<m_reply->readAll().size();
    m_times<<time - m_lasttime;

    m_lasttime = time;

}
