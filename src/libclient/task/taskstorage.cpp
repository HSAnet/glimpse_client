#include "taskstorage.h"
#include "scheduler/scheduler.h"
#include "../storage/storagepaths.h"
#include "../log/logger.h"
#include "task.h"
#include "types.h"

#include <QDir>
#include <QPointer>
#include <QDebug>
#include <QCoreApplication>
#include <QJsonDocument>

LOGGER(TaskStorage);

class TaskStorage::Private : public QObject
{
    Q_OBJECT

public:
    Private()
    : loading(false)
    , dir(StoragePaths().taskDirectory())
    {
        if (!dir.exists())
        {
            if (!QDir::root().mkpath(dir.absolutePath()))
            {
                LOG_ERROR(QString("Unable to create path %1").arg(dir.absolutePath()));
            }
            else
            {
                LOG_DEBUG("Scheduler storage directory created");
            }
        }
    }

    // Properties
    bool loading; // to prevent storing while loading

    QPointer<Scheduler> scheduler;
    QDir dir;

    // Functions
    void store(const Task &task);
    QString fileNameForTask(const Task &task) const;

public slots:
    void taskAdded(const Task &test);
};

void TaskStorage::Private::store(const Task &task)
{
    QJsonDocument document = QJsonDocument::fromVariant(task.toVariant());

    QFile file(dir.absoluteFilePath(fileNameForTask(task)));

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

QString TaskStorage::Private::fileNameForTask(const Task &task) const
{
    return QString::number(task.id().toInt());
}

void TaskStorage::Private::taskAdded(const Task &task)
{

    if (loading)
    {
        return;
    }

    store(task);
}

TaskStorage::TaskStorage(Scheduler *scheduler, QObject *parent)
: QObject(parent)
, d(new Private)
{
    d->scheduler = scheduler;

    connect(scheduler, SIGNAL(taskAdded(Task)), d, SLOT(taskAdded(Task)));
}

TaskStorage::~TaskStorage()
{
    delete d;
}

void TaskStorage::storeData()
{
    foreach (const Task &task, d->scheduler->tasks())
    {
        d->store(task);
    }
}

void TaskStorage::loadData()
{
    d->loading = true;

    foreach (const QString &fileName, d->dir.entryList(QDir::Files))
    {
        QFile file(d->dir.absoluteFilePath(fileName));

        if (!file.open(QIODevice::ReadOnly))
        {
            LOG_DEBUG(QString("Error opening %1: %2").arg(d->dir.absoluteFilePath(fileName)).arg(file.errorString()));
            continue;
        }

        QJsonParseError error;
        QJsonDocument document = QJsonDocument::fromJson(file.readAll(), &error);

        if (error.error == QJsonParseError::NoError)
        {
            Task task = Task::fromVariant(document.toVariant());

            if (!task.isNull())
            {
                d->scheduler->addTask(task);
            }
        }
        else
        {
            LOG_ERROR(QString("Error reading %1: %2").arg(d->dir.absoluteFilePath(fileName)).arg(error.errorString()));
        }
    }

    d->loading = false;
}

#include "taskstorage.moc"
