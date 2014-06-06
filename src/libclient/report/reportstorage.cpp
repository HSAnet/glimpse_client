#include "reportstorage.h"
#include "../storage/storagepaths.h"
#include "../log/logger.h"
#include "types.h"

#include <QPointer>
#include <QDir>
#include <QCoreApplication>
#include <QUuid>
#include <QJsonDocument>
#include <QFile>
#include <QDebug>

LOGGER(ReportStorage);

class ReportStorage::Private : public QObject
{
    Q_OBJECT

public:
    Private()
    : loading(false)
    , dir(StoragePaths().reportDirectory())
    , realTime(true)
    {
        if (!dir.exists())
        {
            if (!QDir::root().mkpath(dir.absolutePath()))
            {
                LOG_ERROR(QString("Unable to create path %1").arg(dir.absolutePath()));
            }
            else
            {
                LOG_INFO("Report storage directory created");
            }
        }
    }

    // Properties
    bool loading;

    QDir dir;
    bool realTime;
    QPointer<ReportScheduler> scheduler;

    // Functions
    void store(const ReportPtr &report);
    QString fileNameForReport(const ReportPtr &report) const;

public slots:
    void reportAdded(const ReportPtr &report);
    void reportModified(const ReportPtr &report);
    void reportRemoved(const ReportPtr &report);
};

void ReportStorage::Private::store(const ReportPtr &report)
{
    QJsonDocument document = QJsonDocument::fromVariant(report->toVariant());

    QFile file(dir.absoluteFilePath(fileNameForReport(report)));

    if (file.open(QIODevice::WriteOnly))
    {
        file.write(document.toJson());
        file.close();
    }
    else
    {
        LOG_ERROR(QString("Unable to open file: %1").arg(file.errorString()));
    }
}

QString ReportStorage::Private::fileNameForReport(const ReportPtr &report) const
{
    return uuidToString(report->taskId());
}

void ReportStorage::Private::reportAdded(const ReportPtr &report)
{
    if (!realTime || loading)
    {
        return;
    }

    store(report);
}

void ReportStorage::Private::reportModified(const ReportPtr &report)
{
    // Treat it like it was added for now
    reportAdded(report);
}

void ReportStorage::Private::reportRemoved(const ReportPtr &report)
{
    if (!realTime)
    {
        return;
    }

    dir.remove(fileNameForReport(report));
}

ReportStorage::ReportStorage(ReportScheduler *scheduler, QObject *parent)
: QObject(parent)
, d(new Private)
{
    d->scheduler = scheduler;

    connect(scheduler, SIGNAL(reportAdded(ReportPtr)), d, SLOT(reportAdded(ReportPtr)));
    connect(scheduler, SIGNAL(reportModified(ReportPtr)), d, SLOT(reportModified(ReportPtr)));
    connect(scheduler, SIGNAL(reportRemoved(ReportPtr)), d, SLOT(reportRemoved(ReportPtr)));
}

ReportStorage::~ReportStorage()
{
    delete d;
}

void ReportStorage::setRealTime(bool realTime)
{
    d->realTime = realTime;
}

bool ReportStorage::isRealTime() const
{
    return d->realTime;
}

void ReportStorage::storeData()
{
    // If realtime was enabled, the files are already on disk so we don't
    // have to modify them again.
    if (d->realTime)
    {
        return;
    }

    foreach (const ReportPtr &report, d->scheduler->reports())
    {
        d->store(report);
    }
}

void ReportStorage::loadData()
{
    d->loading = true;

    foreach (const QString &fileName, d->dir.entryList(QDir::Files))
    {
        QFile file(d->dir.absoluteFilePath(fileName));
        file.open(QIODevice::ReadOnly);

        // TODO: Error checking
        QJsonParseError error;
        QJsonDocument document = QJsonDocument::fromJson(file.readAll(), &error);

        if (error.error == QJsonParseError::NoError)
        {
            ReportPtr report = Report::fromVariant(document.toVariant());
            d->scheduler->addReport(report);
        }
        else
        {
            LOG_ERROR(QString("Error loading file %1: %2").arg(d->dir.absoluteFilePath(fileName)).arg(error.errorString()));
        }
    }

    d->loading = false;
}

#include "reportstorage.moc"
