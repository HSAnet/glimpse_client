#ifndef HTTPGETREQUEST_H
#define HTTPGETREQUEST_H

#include "../measurement.h"
#include "httpdownload_definition.h"

#include <QElapsedTimer>
#include <QHostInfo>
#include <QUrl>
#include <QList>
#include <QThread>
#include <QTimer>
#include <QTcpSocket>
#include <QPointer>


class DownloadThread : public QObject
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

    DownloadThread (const QUrl &url, const QHostInfo &server, int targetTimeMs = 10000, bool avoidCaches = false, QObject *parent = 0);
    ~DownloadThread();

    DownloadThreadStatus threadStatus() const;

    qint64 timeToFirstByteInNs() const;
    qint64 startTimeInNs() const;
    qint64 endTimeInNs() const;
    qint64 runTimeInNs() const;

    qreal averageThroughput(qint64 sTime, qint64 eTime) const; //average througput in bps
    QList<qreal> measurementSlots(int slotLength) const; //slotLength in ms

private:

    //url holds the URL to download from (incl. the port number, default 80)
    QUrl url;
    //IP address of the resolved host in the url above
    QHostInfo server;
    //the time in which the download should finish (from the definition) im ms
    int targetTime;
    //testCaches? true: don't randomize URL, false: randomize URL
    bool avoidCaches;

    //the socket from which to read/write, to be created after moved to thread!
    //that's why we need a pointer here
    QTcpSocket *socket;

    //current status...see enum above
    DownloadThreadStatus tStatus;

    //absolute start time of the download
    QDateTime startTime;
    //absolute end time of the download
    QDateTime endTime;
    //relative time until the first byte was received
    qint64 timeToFirstByte;

    //the measurement Timer for tracking the time slots
    QElapsedTimer measurementTimer;

    //list of bytes received
    QList<qint64> bytesReceived;
    //list of times in which the bytes above were received
    QList<qint64> timeIntervals;

    //some more or less magic constants used
    //TCP timeout on the 3-way handshake in ms
    static const int tcpConnectTimeout = 5000;
    static const int firstByteReceivedTimeout = 5000;
    static const int defaultPort = 80;

public slots:
    //tells the thread to perform the 3-way handshake
    void startTCPConnection();
    void disconnectionHandling();
    void startDownload();
    void stopDownload();
    //reads data from the socket whenever there's data ready to be read
    QByteArray read();

signals:
    void TCPConnected(bool success);
    void TCPDisconnected();
    void firstByteReceived(bool success);
    void downloadFinished(bool success);
};


class HTTPDownload : public Measurement
{
    Q_OBJECT

public:
    explicit HTTPDownload(QObject *parent = 0);
    ~HTTPDownload();

    // Measurement interface
    Status status() const;
    bool prepare(NetworkManager *networkManager, const MeasurementDefinitionPtr &measurementDefinition);
    bool start();
    bool stop();
    Result result() const;

private:
    //results
    QVariantList results;

    void setStatus(Status status);
    bool resultsTrustable();
    bool calculateResults();

    HTTPDownloadDefinitionPtr definition;

    Status currentStatus;
    QUrl requestUrl;

    QList <QThread *> threads;
    QList <QPointer<DownloadThread> > workers;

    QList <qreal> downloadSpeeds;
    qreal overallBandwidth;

    //timer to time out the download (from startTime till targetTime in milli-seconds later)
    QTimer downloadTimer;

    QDateTime downloadStartTime;

    int connectedThreads;   //number of threads that have finished the TCP handshake
    int unconnectedThreads; //number of threads that have _not_ finished the TCP handshake
    int downloadingThreads;
    int notDownloadingThreads;

    int finishedThreads;    //number of threads that have finished the download

    //some more or less magic constants
    static const int maxRampUpTime = 10000; //max ramp-up time in milli-seconds for TCP to grow the CWND
    static const int minRampUpTime = 1000;
    static const int maxThreads = 6;
    static const int minThreads = 1;
    static const int maxTargetTime = 45000; //no download should last longer than that (security reasons)
    static const int minTargetTime = 2000; //so download should be shorter than this, really
    static const int minSlotLength = 250;

private slots:
    bool startThreads(const QHostInfo &server);
    void downloadFinished();

public slots:
    void TCPConnectionTracking(bool success);
    void downloadStartedTracking(bool success);
    void prematureDisconnectedTracking();

signals:
    void statusChanged(Status status);
    void connectTCP();
    void startDownload();
};

#endif // HTTPGETREQUEST_H
