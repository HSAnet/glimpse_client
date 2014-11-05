#include "btc_ma.h"
#include "../../log/logger.h"
#include "../../network/networkmanager.h"
#include "../../client.h"
#include "../../trafficbudgetmanager.h"

#include <QDataStream>
#include <numeric>
#include <QtCore/QtMath>

LOGGER(BulkTransportCapacityMA);

BulkTransportCapacityMA::BulkTransportCapacityMA(QObject *parent)
: Measurement(parent)
, m_preTest(true)
, m_tcpSocket(NULL)
, m_lasttime(-1)
, m_status(Unknown)
{
    connect(this, SIGNAL(error(const QString &)), this,
            SLOT(setErrorString(const QString &)));
}

bool BulkTransportCapacityMA::start()
{
    m_status = BulkTransportCapacityMA::Running;

    LOG_INFO("Sending initial data size to server");
    sendRequest(definition->initialDataSize);

    return true;
}

void BulkTransportCapacityMA::sendRequest(quint64 bytes)
{
    // invalidate timer
    m_time.invalidate();

    m_bytesExpected = bytes;
    QDataStream out(m_tcpSocket);
    out << bytes;
}

void BulkTransportCapacityMA::calculateResult()
{
    // if we are in pretest-phase we need to calculate the bytes for the big test
    if (m_preTest)
    {
        // get the download speed in kbyte
        qreal downloadSpeed = (m_bytesReceived / 1024.0) / (((m_lasttime / 1000.0) / 1000) / 1000);

        LOG_INFO(QString("Speed (pretest): %1 KByte/s").arg(downloadSpeed, 0, 'f', 0));

        // Calculate new test size
        m_bytesExpected = downloadSpeed * 1024 * 3; // should be <3 seconds if the calculated speed was correct

        // Request high amount of data
        LOG_INFO("Sending test data size to server");
        m_preTest = false;

        if (Client::instance()->trafficBudgetManager()->addUsedTraffic(m_bytesExpected))
        {
            sendRequest(m_bytesExpected);
        }
        else
        {
            LOG_ERROR("not enough traffic available");
            emit error("not enough traffic available");
            return;
        }
    }
    else
    {
        // calculate download speed for n slices
        int slices = 10; // TODO 10 slices, as parameter?
        int slice_size = ceil((double)m_times.size() / slices); // round up to get the correct number of slices

        // counters
        qint64 bytes = 0;
        qint64 time = 0;

        // calculate slices
        for (int i = 0; i < m_times.size(); i++)
        {
            bytes += m_bytesReceivedList.at(i);
            time += m_times.at(i);

            // calculate a slice if enough samples are added up or if this is the last iteration
            if (((i + 1) % slice_size) == 0 || i == m_times.size() - 1) // calculate a slice
            {
                qreal speed = (bytes / 1024.0) / (((time / 1000.0) / 1000) / 1000);
                m_downloadSpeeds << speed;

                // reset counters
                bytes = 0;
                time = 0;
            }
        }

        // calculate mean speed
        qreal sum = std::accumulate(m_downloadSpeeds.begin(), m_downloadSpeeds.end(), 0.0);
        qreal mean = sum / m_downloadSpeeds.size();
        LOG_INFO(QString("Speed (mean): %1 KByte/s").arg(mean, 0, 'f', 0));

        /* At the moment deactivated
        // calculate standard deviation
        qreal sq_sum = std::inner_product(speeds.begin(), speeds.end(), speeds.begin(), 0.0);
        qreal stdev = qSqrt(sq_sum / speeds.size() - mean * mean);
        LOG_INFO(QString("Standard deviation: %1").arg(stdev, 0, 'f', 0));
        */

        /* At the moment deactivated to get all results
        // get ride of the lower and upper 20%
        qSort(speeds);
        int percentil = slices * 0.2;
        sum = std::accumulate(speeds.begin()+percentil, speeds.end()-percentil, 0.0);
        mean = sum / (speeds.size() - 2 * percentil);
        LOG_INFO(QString("Speed (60 percentil mean): %1 KByte/s").arg(mean, 0, 'f', 0));
        */

        m_status = BulkTransportCapacityMA::Finished;

        emit finished();
    }
}

void BulkTransportCapacityMA::receiveResponse()
{
    qint64 time = m_time.nsecsElapsed();

    // if the timer is not valid this is the first response packet
    if (!m_time.isValid())
    {
        // reset variables
        m_bytesReceived = 0;
        m_lasttime = 0;

        // this packet counts to the total bytes, but not to the measurement bytes in m_bytesReceived
        m_totalBytesReceived = m_tcpSocket->bytesAvailable();

        // ignor the first data received
        m_tcpSocket->readAll();

        m_time.restart();
    }
    else // this is a response within an active measurement
    {
        qint64 bytes = m_tcpSocket->bytesAvailable();
        m_bytesReceivedList << bytes;
        m_times << time - m_lasttime;
        m_bytesReceived += bytes;
        m_totalBytesReceived += bytes;
        m_tcpSocket->readAll(); // we don't care for the data-content
        m_lasttime = time;
    }

    // check if all measurement data was received
    if (m_totalBytesReceived >= m_bytesExpected)
    {
        calculateResult();
    }
}

void BulkTransportCapacityMA::serverDisconnected()
{
    if (m_status != BulkTransportCapacityMA::Finished)
    {
        LOG_WARNING("Server closed connection, this should not happen");
    }
}

void BulkTransportCapacityMA::handleError(QAbstractSocket::SocketError socketError)
{
    if (socketError == QAbstractSocket::RemoteHostClosedError)
    {
        return;
    }

    QAbstractSocket *socket = qobject_cast<QAbstractSocket *>(sender());
    LOG_ERROR(QString("Socket Error: %1").arg(socket->errorString()));
    emit error(socket->errorString());
}

Measurement::Status BulkTransportCapacityMA::status() const
{
    return m_status;
}

bool BulkTransportCapacityMA::prepare(NetworkManager *networkManager,
                                      const MeasurementDefinitionPtr &measurementDefinition)
{
    definition = measurementDefinition.dynamicCast<BulkTransportCapacityDefinition>();

    if (definition.isNull())
    {
        setErrorString("Definition is empty");
        return false;
    }

    QString hostname = QString("%1:%2").arg(definition->host).arg(definition->port);

    m_tcpSocket = qobject_cast<QTcpSocket *>(networkManager->establishConnection(hostname, taskId(), "btc_mp", definition,
                                                                                 NetworkManager::TcpSocket));

    if (!m_tcpSocket)
    {
        setErrorString("Preparation failed");
        return false;
    }

    m_tcpSocket->setParent(this);
    m_bytesExpected = 0;
    m_preTest = true;

    if (!Client::instance()->trafficBudgetManager()->addUsedTraffic(definition->initialDataSize))
    {
        setErrorString("not enough traffic available");
        return false;
    }

    // Signal for new data
    connect(m_tcpSocket, SIGNAL(readyRead()), this, SLOT(receiveResponse()));

    // Signal for errors
    connect(m_tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this,
            SLOT(handleError(QAbstractSocket::SocketError)));

    // Signal for end of data transmission
    connect(m_tcpSocket, SIGNAL(disconnected()), this, SLOT(serverDisconnected()));

    return true;
}

bool BulkTransportCapacityMA::stop()
{
    if (m_tcpSocket)
    {
        m_tcpSocket->disconnectFromHost();

        if (m_tcpSocket->state() != QTcpSocket::UnconnectedState &&
            !m_tcpSocket->waitForDisconnected(1000))
        {
            return false;
        }
    }

    return true;
}

Result BulkTransportCapacityMA::result() const
{
    QVariantMap res;
    QVariantList downSpeeds;
    float avg = 0.0;

    // get max and min
    QList<qreal>::const_iterator it = std::max_element(m_downloadSpeeds.begin(), m_downloadSpeeds.end());
    float max = *it;
    it = std::min_element(m_downloadSpeeds.begin(), m_downloadSpeeds.end());
    float min = *it;

    // calculate average and fill downspeeds
    foreach (qreal val, m_downloadSpeeds)
    {
        downSpeeds << val;
        avg += val;
    }

    avg /= m_downloadSpeeds.size();

    // calculate standard deviation
    qreal sq_sum = std::inner_product(m_downloadSpeeds.begin(), m_downloadSpeeds.end(), m_downloadSpeeds.begin(), 0.0);
    qreal stdev = qSqrt(sq_sum / m_downloadSpeeds.size() - avg * avg);

    res.insert("kBs_avg", avg);
    res.insert("kBs_min", min);
    res.insert("kBs_max", max);
    res.insert("kBs_stddev", stdev);
    res.insert("kBs", downSpeeds);

    return Result(res, definition->measurementUuid);
}
