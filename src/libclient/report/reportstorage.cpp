#include "reportstorage.h"

#include <QPointer>
#include <QDir>
#include <QCoreApplication>
#include <QUuid>
#include <QJsonDocument>
#include <QFile>
#include <QDebug>

class ReportStorage::Private : public QObject
{
    Q_OBJECT

public:
    Private()
     : dir(qApp->applicationDirPath())
     ,realTime(true)
    {
        dir.mkdir("reports");
        dir.cd("reports");
    }

    // Properties
    QDir dir;
    bool realTime;
    QPointer<ReportScheduler> scheduler;

    // Functions
    void store(const ReportPtr& report);
    QString fileNameForReport(const ReportPtr& report) const;

public slots:
    void reportAdded(const ReportPtr& report);
    void reportRemoved(const ReportPtr& report);
};

void ReportStorage::Private::store(const ReportPtr &report)
{
    QJsonDocument document = QJsonDocument::fromVariant(report->toVariant());

    QFile file(dir.absoluteFilePath(fileNameForReport(report)));
    if (file.open(QIODevice::WriteOnly)) {
        file.write(document.toJson());
        file.close();
    } else {
        qDebug() << "Unable to open file:" << file.errorString();
    }
}

QString ReportStorage::Private::fileNameForReport(const ReportPtr &report) const
{
    return report->taskId().toString();
}

void ReportStorage::Private::reportAdded(const ReportPtr &report)
{
    if ( !realTime )
        return;

    store(report);
}

void ReportStorage::Private::reportRemoved(const ReportPtr &report)
{
    if ( !realTime )
        return;

    dir.remove(fileNameForReport(report));
}

ReportStorage::ReportStorage(ReportScheduler *scheduler, QObject *parent)
: QObject(parent)
, d(new Private)
{
    d->scheduler = scheduler;

    connect(scheduler, SIGNAL(reportAdded(ReportPtr)), d, SLOT(reportAdded(ReportPtr)));
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
        return;

    foreach(const ReportPtr& report, d->scheduler->reports())
        d->store(report);
}

void ReportStorage::loadData()
{
    foreach(const QString& fileName, d->dir.entryList(QDir::Files)) {
        QFile file(d->dir.absoluteFilePath(fileName));
        file.open(QIODevice::ReadOnly);

        // TODO: Error checking
        QJsonDocument document = QJsonDocument::fromJson(file.readAll());
        ReportPtr report = Report::fromVariant(document.toVariant());

        d->scheduler->addReport(report);
    }
}

#include "reportstorage.moc"
