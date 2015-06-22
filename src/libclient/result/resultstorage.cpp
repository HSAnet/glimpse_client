#include "resultstorage.h"
#include "../storage/storagepaths.h"
#include "../log/logger.h"
#include "types.h"
#include "../report/report.h"

#include <QPointer>
#include <QDir>
#include <QCoreApplication>
#include <QUuid>
#include <QJsonDocument>
#include <QFile>
#include <QDebug>

LOGGER(ResultStorage);

class ResultStorage::Private : public QObject
{
    Q_OBJECT

public:
    Private()
    : loading(false)
    // TODO: change to resultDirectory
    , dir(StoragePaths().localCopyDirectory())
    {
        if (!dir.exists())
        {
            if (!QDir::root().mkpath(dir.absolutePath()))
            {
                LOG_ERROR(QString("Unable to create path %1").arg(dir.absolutePath()));
            }
            else
            {
                LOG_DEBUG("Result storage directory created");
            }
        }
    }

    // Properties
    bool loading;

    QDir dir;
    QPointer<ResultScheduler> scheduler;
    QPointer<ReportScheduler> reportScheduler;

    // Functions
    void store(const Report &report);
    void store(const QVariantMap &result);
    QVariantList loadResults(const Report &report) const;
    QString fileNameForResult(const Report &report) const;

public slots:
    void reportAdded(const Report &report);
};

void ResultStorage::Private::store(const Report &report)
{
    QVariantList results(loadResults(report));

    if (report.results().length() > 0)
    {
        // only add the last (latest) result to avoid duplicates
        results.append(report.results()[report.results().length()-1].toVariantStripped());
    }

    QJsonDocument document = QJsonDocument::fromVariant(results);
    QFile file(dir.absoluteFilePath(fileNameForResult(report)));

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

QVariantList ResultStorage::Private::loadResults(const Report &report) const
{
    QVariantList out;
    QFile file(dir.absoluteFilePath(fileNameForResult(report)));

    if (!file.exists())
    {
        return out;
    }

    file.open(QIODevice::ReadOnly);

    // Error checking
    QJsonParseError error;
    QJsonDocument document = QJsonDocument::fromJson(file.readAll(), &error);

    if (error.error == QJsonParseError::NoError)
    {
        out = document.toVariant().toList();
    }
    else
    {
        LOG_ERROR(QString("Error loading file %1: %2").arg(file.fileName()).arg(error.errorString()));
    }

    return out;
}
QString ResultStorage::Private::fileNameForResult(const Report &report) const
{
    return QString("%1_%2.json").arg(QString::number(report.taskId().toInt())).arg(
                                     QDateTime::currentDateTime().date().toString("yyyy-MM-dd"));
}

void ResultStorage::Private::reportAdded(const Report &report)
{
    if (loading)
    {
        return;
    }

    store(report);

    QVariantMap map;

    map.insert("task_id", report.taskId().toInt());
    map.insert("results", report.results()[report.results().length()-1].toVariant());
    scheduler->addResult(map);
}

ResultStorage::ResultStorage(ResultScheduler *scheduler, ReportScheduler *reportScheduler, QObject *parent)
: QObject(parent)
, d(new Private)
{
    d->scheduler = scheduler;
    d->reportScheduler = reportScheduler;
}

void ResultStorage::init()
{
    connect(d->reportScheduler, SIGNAL(reportAdded(Report)), d, SLOT(reportAdded(Report)));
    connect(d->reportScheduler, SIGNAL(reportModified(Report)), d, SLOT(reportAdded(Report)));
}

ResultStorage::~ResultStorage()
{
    delete d;
}

void ResultStorage::loadData()
{
    d->loading = true;

    foreach (const QString &fileName, d->dir.entryList(QDir::Files))
    {
        QFile file(d->dir.absoluteFilePath(fileName));
        file.open(QIODevice::ReadOnly);

        // read TaskId from filename as this is needed for the result page
        QStringList fn = fileName.split("_");
        TaskId taskId(fn[0].toInt());

        // Error checking
        QJsonParseError error;
        QJsonDocument document = QJsonDocument::fromJson(file.readAll(), &error);

        if (error.error == QJsonParseError::NoError)
        {
            QVariantList list = document.toVariant().toList();
            QVariantMap result;

            foreach (const QVariant &variant, list)
            {
                result.insert("task_id", taskId.toInt());
                result.insert("results", variant);
                d->scheduler->addResult(result);
            }
        }
        else
        {
            LOG_ERROR(QString("Error loading file %1: %2").arg(d->dir.absoluteFilePath(fileName)).arg(error.errorString()));
        }
    }

    d->loading = false;
}

#include "resultstorage.moc"
