#include "schedulerstorage.h"
#include "scheduler.h"

#include <QDir>
#include <QPointer>
#include <QDebug>
#include <QCoreApplication>
#include <QJsonDocument>

class SchedulerStorage::Private : public QObject
{
    Q_OBJECT

public:
    Private()
    : dir(qApp->applicationDirPath())
    , realTime(true)
    {
        connect(scheduler.data(), SIGNAL(testAdded(TestDefinition,int)), this, SLOT(testAdded(TestDefinition,int)));
        connect(scheduler.data(), SIGNAL(testRemoved(TestDefinition,int)), this, SLOT(testRemoved(TestDefinition,int)));

        dir.mkdir("scheduler");
        dir.cd("scheduler");
    }

    // Properties
    QPointer<Scheduler> scheduler;
    QDir dir;
    bool realTime;

    // Functions
    void store(const TestDefinition& test);
    QString fileNameForTest(const TestDefinition& test) const;

public slots:
    void testAdded(const TestDefinition& test, int position);
    void testRemoved(const TestDefinition& test, int position);
};

void SchedulerStorage::Private::store(const TestDefinition& test)
{
    QJsonDocument document = QJsonDocument::fromVariant(test.toVariant());

    QFile file(dir.absoluteFilePath(fileNameForTest(test)));
    if (file.open(QIODevice::WriteOnly)) {
        file.write(document.toJson());
        file.close();
    } else {
        qDebug() << "Unable to open file:" << file.errorString();
    }
}

QString SchedulerStorage::Private::fileNameForTest(const TestDefinition& test) const
{
    return test.id.toString();
}

void SchedulerStorage::Private::testAdded(const TestDefinition& test, int position)
{
    Q_UNUSED(position);

    if (!realTime)
        return;

    store(test);
}

void SchedulerStorage::Private::testRemoved(const TestDefinition& test, int position)
{
    Q_UNUSED(position);

    if (!realTime)
        return;

    dir.remove(fileNameForTest(test));
}

SchedulerStorage::SchedulerStorage(Scheduler *scheduler, QObject *parent)
: QObject(parent)
, d(new Private)
{
    d->scheduler = scheduler;
}

SchedulerStorage::~SchedulerStorage()
{
}

void SchedulerStorage::setRealTime(bool realTime)
{
    d->realTime = realTime;
}

bool SchedulerStorage::isRealtime() const
{
    return d->realTime;
}

void SchedulerStorage::storeData()
{
    foreach(const TestDefinition& test, d->scheduler->tests())
        d->store(test);
}

void SchedulerStorage::loadData()
{
    foreach(const QString& fileName, d->dir.entryList(QDir::Files)) {
        QFile file(d->dir.absoluteFilePath(fileName));
        file.open(QIODevice::ReadOnly);

        // TODO: Error checking
        QJsonDocument document = QJsonDocument::fromJson(file.readAll());
        TestDefinition test = TestDefinition::fromVariant(document.toVariant());

        d->scheduler->enqueue(test);
    }
}

#include "schedulerstorage.moc"
