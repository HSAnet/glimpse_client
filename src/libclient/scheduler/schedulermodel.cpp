#include "schedulermodel.h"
#include "scheduler.h"

#include <QPointer>
#include <QTimer>

class SchedulerModel::Private : public QObject
{
    Q_OBJECT

public:
    Private(SchedulerModel* q)
    : q(q)
    {
        QTimer* timer = new QTimer(this);
        connect(timer, SIGNAL(timeout()), this, SLOT(update()));
        timer->start(100);
    }

    SchedulerModel* q;

    QPointer<Scheduler> scheduler;
    TestDefinitionList tests;

public slots:
    void update();

    void testAdded(const TestDefinition& test, int position);
    void testRemoved(const TestDefinition& test, int position);
    void testMoved(const TestDefinition& test, int from, int to);
};

void SchedulerModel::Private::update()
{
    emit q->dataChanged(q->index(0,0), q->index(q->rowCount(QModelIndex()), q->columnCount(QModelIndex())));
}

void SchedulerModel::Private::testAdded(const TestDefinition& test, int position)
{
    q->beginInsertRows(QModelIndex(), position, position);
    tests.insert(position, test);
    q->endInsertRows();
}

void SchedulerModel::Private::testRemoved(const TestDefinition& test, int position)
{
    Q_UNUSED(test);

    q->beginRemoveRows(QModelIndex(), position, position);
    tests.removeAt(position);
    q->endRemoveRows();
}

void SchedulerModel::Private::testMoved(const TestDefinition& test, int from, int to)
{
    Q_UNUSED(test);

    q->beginMoveRows(QModelIndex(), from, from,
                     QModelIndex(), to+1);
    tests.move(from, to);
    q->endMoveRows();
}

SchedulerModel::SchedulerModel(QObject *parent)
: QAbstractTableModel(parent)
, d(new Private(this))
{
}

SchedulerModel::~SchedulerModel()
{
    delete d;
}

void SchedulerModel::setScheduler(Scheduler *scheduler)
{
    if (d->scheduler == scheduler)
        return;

    if (d->scheduler) {
        disconnect(d->scheduler.data(), SIGNAL(testAdded(TestDefinition,int)), this, SLOT(testAdded(TestDefinition,int)));
        disconnect(d->scheduler.data(), SIGNAL(testRemoved(TestDefinition,int)), this, SLOT(testRemoved(TestDefinition,int)));
        disconnect(d->scheduler.data(), SIGNAL(testMoved(TestDefinition,int,int)), this, SLOT(testMoved(TestDefinition,int,int)));
    }

    d->scheduler = scheduler;

    if (d->scheduler) {
        connect(d->scheduler.data(), SIGNAL(testAdded(TestDefinition,int)), this, SLOT(testAdded(TestDefinition,int)));
        connect(d->scheduler.data(), SIGNAL(testRemoved(TestDefinition,int)), this, SLOT(testRemoved(TestDefinition,int)));
        connect(d->scheduler.data(), SIGNAL(testMoved(TestDefinition,int,int)), this, SLOT(testMoved(TestDefinition,int,int)));
    }

    reset();
}

Scheduler *SchedulerModel::scheduler() const
{
    return d->scheduler;
}

void SchedulerModel::reset()
{
    beginResetModel();

    if (!d->scheduler.isNull())
        d->tests = d->scheduler->tests();

    endResetModel();
}

QVariant SchedulerModel::get(int index) const
{
    QModelIndex idx = this->index(index, 0);
    if (!idx.isValid())
        return QVariant();

    QVariantMap hash;
    QHashIterator<int, QByteArray> iter(roleNames());
    while(iter.hasNext()) {
        iter.next();
        hash.insert(iter.value(), data(idx, iter.key()));
    }

    return hash;
}

QHash<int, QByteArray> SchedulerModel::roleNames() const
{
    QHash<int, QByteArray> roleNames;
    roleNames.insert(NameRole, "name");
    roleNames.insert(NextRunRole, "nextRun");
    roleNames.insert(TimeLeftRole, "timeLeft");
    roleNames.insert(TypeRole, "type");
    roleNames.insert(IdRole, "id");
    return roleNames;
}

int SchedulerModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return d->tests.size();
}

int SchedulerModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 5;
}

QVariant SchedulerModel::data(const QModelIndex &index, int role) const
{
    const TestDefinition& testDefinition = d->tests.at(index.row());

    if (role == Qt::DisplayRole)
        role = NameRole + index.column();

    switch(role) {
    case NameRole: return testDefinition.name;
    case NextRunRole: return testDefinition.timing->nextRun();
    case TimeLeftRole: return testDefinition.timing->timeLeft();
    case TypeRole: return testDefinition.timing->type();
    case IdRole: return testDefinition.id;
    }

    return QVariant();
}

QVariant SchedulerModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole || orientation != Qt::Horizontal)
        return QVariant();

    section += NameRole;

    switch(section) {
    case NameRole: return tr("Name");
    case NextRunRole: return tr("Next Run");
    case TimeLeftRole: return tr("Time left");
    case TypeRole: return tr("Type");
    case IdRole: return tr("Id");

    default:
        return QVariant();
    }
}
