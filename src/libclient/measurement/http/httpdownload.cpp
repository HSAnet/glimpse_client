#include "httpdownload.h"
#include "../../log/logger.h"
#include "types.h"

#include <QtMath>
#include <numeric>

LOGGER(HTTPDownload);

DownloadThread::DownloadThread (const QUrl &url, const QHostInfo &server, int targetTimeMs, bool cacheTest, QObject *parent)
: QObject(parent)
, url(url)
, server(server)
, targetTime(targetTimeMs)
, avoidCaches(cacheTest)
, socket(NULL)
, tStatus(Inactive)
{
}

DownloadThread::~DownloadThread()
{
    //socket needs to be deleted
    if (socket != NULL)
    {
        //shouldn't happen, but in case it does we are good
        //Internet citizens and behave nicely
        if (socket->state() == QAbstractSocket::ConnectedState)
        {
            socket->close();
        }

        delete socket;
    }
}

DownloadThread::DownloadThreadStatus DownloadThread::threadStatus() const
{
    return tStatus;
}

qint64 DownloadThread::timeToFirstByteInNs() const
{
    return timeToFirstByte;
}

qint64 DownloadThread::startTimeInNs() const
{
    return startTime.toMSecsSinceEpoch() * 1000000;
}

qint64 DownloadThread::endTimeInNs() const
{
    return startTime.toMSecsSinceEpoch() * 1000000 + timeIntervals.last();
}

qint64 DownloadThread::runTimeInNs() const
{
    return timeIntervals.last();
}

void DownloadThread::startTCPConnection()
{
    //each Thread is supposed to first build up the TCP connection,
    //emit the connected signal, and only when all threads have connected
    //do the actual download (coordinated by HTTPDownload)

    //shouldn't happen, check anyway
    if (server.addresses().isEmpty() || (!url.isValid()))
    {
        //invoke the connection tracking code
        tStatus = FinishedError;
        return;
    }

    socket = new QTcpSocket();

    //so let's connect now (if no port as part of the URL use 80 as default)
    socket->connectToHost(server.addresses().first(), url.port(defaultPort));

    tStatus = ConnectingTCP;

    //wait for up to 5 seconds for a successful connection
    if (socket->waitForConnected(tcpConnectTimeout))
    {
        //for the successfully connected sockets, we should track the disconnection
        connect(socket, &QTcpSocket::disconnected, this, &DownloadThread::disconnectionHandling);
        tStatus = ConnectedTCP;
        emit TCPConnected(true);
    }
    else
    {
        //something went wrong
        tStatus = FinishedError;
        emit TCPConnected(false);
        socket->close();
    }
}

void DownloadThread::disconnectionHandling()
{
    // handling premature TCP disconnects
    if(tStatus == DownloadInProgress)
    {
        tStatus = FinishedSuccess;
        emit TCPDisconnected();
    }
}

void DownloadThread::startDownload()
{
    //we can only download, if this thread sucessfully established the
    //TCP connection (all threads will receive the signal)
    if(socket->state() != QAbstractSocket::ConnectedState)
    {
        tStatus = FinishedError;
        emit firstByteReceived(false);
        return;
    }

    QString path = url.path();

    if (avoidCaches)
    {
        path.append(QString("?timestamp=%1").arg(QDateTime::currentDateTime().toString("yy_MM_dd_HH_mm_ss_zzz")));
    }

    QString request = QString("GET %1 HTTP/1.1\r\n"
                              "Host: %2\r\n"
                              "User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10.9; rv:31.0) Gecko/20100101 Firefox/31.0\r\n"
                              "Referer: http://www.measure-it.net\r\n\r\n").arg(path).arg(url.host());

    int bytesWritten = 0;

    //log actual time of start
    startTime = QDateTime::currentDateTime();

    //send the HTTP GET
    while(bytesWritten < request.length())
    {
        bytesWritten += socket->write(request.mid(bytesWritten).toLatin1());

        //on error
        if(bytesWritten < 0)
        {
            socket->close();
            tStatus = FinishedError;
            emit firstByteReceived(false);
            return;
        }
    }

    //start eplapsed timer for calculating the time slots
    measurementTimer.start();

    tStatus = AwaitingFirstByte;

    //do a blocking read for the first bytes or time-out if nothing is arriving
    if (socket->waitForReadyRead(firstByteReceivedTimeout))
    {
        //TODO: check for HTTP status code and act intelligently on it
        //currently, a 404 etc. is simply to little data
        //to generate results, but a better checking would be great
        timeToFirstByte = measurementTimer.nsecsElapsed();
        tStatus = DownloadInProgress;
        //connect readyRead of the socket with read() for further reads
        connect(socket, &QTcpSocket::readyRead, this, &DownloadThread::read);
        //call read
        read();
        emit firstByteReceived(true);
    }
    else
    {
        tStatus = FinishedError;
        socket->close();
        emit firstByteReceived(false);
    }
}

void DownloadThread::read()
{
    bytesReceived << socket->bytesAvailable();
    timeIntervals << measurementTimer.nsecsElapsed();

    socket->readAll();   //we don't need the actual data but need to free space in the
                         //socket buffer
}

qreal DownloadThread::averageThroughput(qint64 sTime, qint64 eTime) const
{
    int i = 0;

    qint64 bytes = 0;

    qint64 begin = sTime - startTimeInNs();
    qint64 end = eTime - startTimeInNs();

    int startSlot = -1;
    int endSlot = -1;

    for (i = 0; i < timeIntervals.size(); i++)
    {
        //start after "begin"
        if (timeIntervals[i] < begin)
        {
            continue;
        }

        if (startSlot < 0)
        {
            startSlot = i;
        }

        if (timeIntervals[i] > end)
        {
            break;
        }

        endSlot = i;

        bytes += bytesReceived[i];
    }

    if (endSlot < 0 || startSlot < 0)
    {
        //this should only happen is we have a wrong time window
        return 0.0;
    }

    return (8.0 * (qreal)bytes)/(((qreal)(timeIntervals[endSlot] - timeIntervals[startSlot]))/1000000000.0);
}

QList<qreal> DownloadThread::measurementSlots(int slotLength) const
{
    int i = 0;

    QList<qreal> slotList;

    int bytes = 0;
    qint64 currentSlotTime = slotLength * 1000000;
    int lastSlot = 0;

    for (i = 0; i < timeIntervals.size(); i++)
    {
        if (timeIntervals[i] > currentSlotTime && i != 0)
        {
            slotList << ((qreal)bytes * 8) / ((timeIntervals[i-1] - timeIntervals[lastSlot])/1000000000.0);

            bytes = bytesReceived[i];
            currentSlotTime += slotLength * 1000000;
            if (timeIntervals[i] > currentSlotTime)
            {
                currentSlotTime = timeIntervals[i];
            }
            lastSlot = i - 1;
        }
        else
        {
            bytes += bytesReceived[i];
        }
    }

    return slotList;
}

void DownloadThread::stopDownload()
{
    if (tStatus == DownloadInProgress)
    {
        tStatus = FinishedSuccess;
    }

    //stop download and clean-up
    if (socket->state() != QAbstractSocket::ConnectedState)
    {
        socket->close();
    }
}




HTTPDownload::HTTPDownload(QObject *parent)
: Measurement(parent)
, currentStatus(HTTPDownload::Unknown)
, overallBandwidth(0.0)
, connectedThreads(0)
, unconnectedThreads(0)
, downloadingThreads(0)
, notDownloadingThreads(0)
, finishedThreads(0)
{
    connect(this, SIGNAL(error(const QString &)), this,
            SLOT(setErrorString(const QString &)));
    setResultHeader(QStringList() << "actual_num_threads" << "results_ok" << "bandwidth_bps_avg" <<
                    "bandwidth_bps_per_thread");
}

HTTPDownload::~HTTPDownload()
{
    //deleting workers not needed - done by deleteLater already
    qDeleteAll(threads);
}

Measurement::Status HTTPDownload::status() const
{
    return currentStatus;
}

//minimal, only create the QUrl object and test bounds on the definiton variables
bool HTTPDownload::prepare(NetworkManager *networkManager,
                           const MeasurementDefinitionPtr &measurementDefinition)
{
    Q_UNUSED(networkManager)

    definition = measurementDefinition.dynamicCast<HTTPDownloadDefinition>();

    if (definition.isNull())
    {
        setErrorString("received NULL definition");
        return false;
    }

    if (definition->threads > maxThreads || definition->threads < minThreads)
    {
        setErrorString("requested number of threads wrong");
        return false;
    }

    if (definition->rampUpTime > maxRampUpTime || definition->rampUpTime < minRampUpTime)
    {
        setErrorString("requested ramp-up time wrong");
        return false;
    }

    if (definition->targetTime > maxTargetTime || definition->targetTime < minTargetTime)
    {
        setErrorString("requested target time wrong");
        return false;
    }

    if (definition->slotLength > definition->targetTime || definition->slotLength < minSlotLength)
    {
        setErrorString("requested slot length wrong");
        return false;
    }

    //set the URL to be used by all threads
    //use a QUrl object to have its convenience functions at hand later
    //do not use setUrl! will not produce proper results e.g. for www.domain-name.tld etc.
    requestUrl = QUrl::fromUserInput(definition->url);

    if (!requestUrl.isValid())
    {
        setErrorString("invalid URL");
        return false;
    }

    return true;
}

bool HTTPDownload::start()
{
    //do the DNS lookup first so that we can pass the IP to each
    //thread instead of having each Thread do the DNS lookup by itself

    //TODO: add a timer to check wheather this has actually gone through or not

    //when the lookup finishes, we want to call the startThreads() function
    //that starts the actual measurement/threads 

    QHostInfo::lookupHost(requestUrl.host(), this, SLOT(startThreads(QHostInfo)));

    return true;
}

//this function starts the actual measurement
bool HTTPDownload::startThreads(const QHostInfo &server)
{
    //check if the name resolution was actually successful
    if (server.error() != QHostInfo::NoError)
    {
        emit error("Name resolution failed");
        return false;
    }

    int n = 0;

    //start all threads
    for(n = 0; n < definition->threads; n++)
    {
        //create a worker thread that starts an actual download
        DownloadThread *worker = new DownloadThread(requestUrl, server, definition->targetTime, definition->avoidCaches);
        QThread *workerThread = new QThread();

        //store the references to the threads/workers
        workers.append(worker);
        threads.append(workerThread);

        //move the worker to its own thread context
        worker->moveToThread(workerThread);

        //when the thread finishes, do some cleanup
        connect(workerThread, &QThread::finished, worker, &QObject::deleteLater);

        //this signal tells a worker thread to initiate the TCP 3-way handshake
        connect(this, &HTTPDownload::connectTCP, worker, &DownloadThread::startTCPConnection);

        //this signal is for the this thread of execution to track the TCP connection state
        //of the worker threads
        connect(worker, &DownloadThread::TCPConnected, this, &HTTPDownload::TCPConnectionTracking);

        //this signal tells the threads to start the actual download
        connect(this, &HTTPDownload::startDownload, worker, &DownloadThread::startDownload);

        connect(worker, &DownloadThread::firstByteReceived, this, &HTTPDownload::downloadStartedTracking);

        connect(worker, &DownloadThread::TCPDisconnected, this, &HTTPDownload::prematureDisconnectedTracking);

        //start the thread
        workerThread->start();
    }

    //now the actual measurement starts
    setStatus(HTTPDownload::Running);

    //tell the threads to do the 3way-handshake
    emit connectTCP();

    return true;
}


void HTTPDownload::prematureDisconnectedTracking()
{
    finishedThreads++;

    if (finishedThreads == connectedThreads)
    {
        downloadFinished();
    }
}

//TCP connection tracking is _only_ for the initial connection
//not for live accounting of the TCP state
//we need to know when to emit download (that's all really)
void HTTPDownload::TCPConnectionTracking(bool success)
{
    if (success)
    {
        connectedThreads++;
    }
    else
    {
        unconnectedThreads++;
    }

    //if that was the last thread to establish a TCP connection (or failed)
    //then start the download
    if (connectedThreads + unconnectedThreads == definition->threads)
    {
        if(connectedThreads == 0)
        {
            emit error("Unable to establish a TCP connection");
            return;
        }

        emit startDownload();
    }
}

void HTTPDownload::downloadStartedTracking(bool success)
{
    if(success)
    {
        downloadingThreads++;
    }
    else
    {
        notDownloadingThreads++;
    }

    //once all threads started their download we can start the download timer
    //plus some ramp-up time for the TCP connection
    //need to check against definition->threads, since the unconnected
    //threads also emit firstByteReceived(false)
    if(downloadingThreads + notDownloadingThreads == definition->threads)
    {
        if(notDownloadingThreads == definition->threads)
        {
            emit error("No thread able to download after TCP connection was established.");
            return;
        }

        downloadStartTime = QDateTime::currentDateTime().addMSecs(definition->rampUpTime);
        //when this timer fires we stop all downloads
        downloadTimer.singleShot(definition->targetTime + definition->rampUpTime, this, SLOT(downloadFinished()));
    }
}

void HTTPDownload::downloadFinished()
{
    bool resultsOK = false;

    //stop the timer if still running (e.g. the case if all
    //threads stop prematurely)
    if(downloadTimer.isActive())
    {
        downloadTimer.stop();
    }

    setStatus(HTTPDownload::Finished);

    int i = 0;
    //stop all threads downloading data
    for(i = 0; i < workers.size(); i++)
    {
        //won't need signals from threads anymore
        workers[i]->disconnect();
        workers[i]->stopDownload();
    }

    resultsOK = calculateResults();

    //clean up (threads etc.) before emitting finished,
    //otherwise this thing can become deleted before the
    //contained threads
    for (i = 0; i < threads.size(); i++)
    {
        threads[i]->quit();
        threads[i]->wait();
    }

    if(resultsOK)
    {
        emit finished();
    }
    else
    {
        emit error("Unable to calculate accurate results on the measurement.");
    }
}

//we ony trust the results if the threads have measured something useful
bool HTTPDownload::resultsTrustable()
{
    //only if _all_ successfully finished threads have a
    //measurement period that is 75% of the envisaged download-time
    //we mark the results as trustable
    int i = 0;

    int unfinishedThreads = 0;

    for (i = 0; i < workers.size(); i++)
    {
        if(workers[i]->threadStatus() != DownloadThread::FinishedSuccess)
        {
            unfinishedThreads++;
            continue;
        }

        //if run time during the measurement period of _all_ threads is above
        //75% of the target time, we assume the measure
        if(workers[i]->runTimeInNs() - \
                (downloadStartTime.toMSecsSinceEpoch() * 1000000 - workers[i]->startTimeInNs())
                < (((double)definition->targetTime * 1000000) * 0.75))
        {
           return false;
        }
    }

    if(unfinishedThreads == definition->threads)
    {
        return false;
    }
    else
    {
        return true;
    }
}


//calculate the overall download speed amongst other things
bool HTTPDownload::calculateResults()
{
    int num_threads = 0;

    QVariantList threadResults;

    bool resultsOK = resultsTrustable();

    for(int i = 0; i < workers.size(); i++)
    {
        //only consider threads that finished successfully
        if(workers[i]->threadStatus() != DownloadThread::FinishedSuccess)
        {
            continue;
        }

        num_threads++;

        QVariantMap thread;
        qreal avg = workers[i]->averageThroughput(downloadStartTime.toMSecsSinceEpoch() * 1000000, \
                                                  downloadStartTime.toMSecsSinceEpoch() * 1000000 + \
                                                  ((qint64) (definition->targetTime)) * 1000000);
        thread.insert("avg", avg);
        overallBandwidth += avg;

        QList<qreal> measurementSlots = workers[i]->measurementSlots(definition->slotLength);

        // get max and min
        QList<qreal>::const_iterator it = std::max_element(measurementSlots.begin(), measurementSlots.end());
        qreal max = *it;
        thread.insert("max", max);
        it = std::min_element(measurementSlots.begin(), measurementSlots.end());
        qreal min = *it;
        thread.insert("min", min);

        qreal sq_sum = std::inner_product(measurementSlots.begin(), measurementSlots.end(), measurementSlots.begin(), 0.0);
        qreal stdev = 0.0;

        if (measurementSlots.size() > 0)
        {
            // calculate standard deviation
            stdev = qSqrt(sq_sum / measurementSlots.size() - avg * avg);
            thread.insert("stdev", stdev);
        }

        thread.insert("slots", listToVariant(measurementSlots));

        threadResults.append(thread);
    }

    results.append(num_threads);

    results.append(resultsOK);

    results.append(overallBandwidth);

    results.append(QVariant(threadResults));

    return true;
}

bool HTTPDownload::stop()
{
    return true;
}

Result HTTPDownload::result() const
{
    return Result(results);
}

void HTTPDownload::setStatus(Status status)
{
    if (currentStatus != status)
    {
        currentStatus = status;
        emit statusChanged(status);
    }
}



