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
    }

    SchedulerModel* q;

    QPointer<Scheduler> scheduler;
    TestDefinitionList tests;

public slots:
    void testAdded(const TestDefinitionPtr &test, int position);
    void testRemoved(const TestDefinitionPtr &test, int position);
    void testMoved(const TestDefinitionPtr &test, int from, int to);
};

void SchedulerModel::Private::testAdded(const TestDefinitionPtr& test, int position)
{
    q->beginInsertRows(QModelIndex(), position, position);
    tests.insert(position, test);
    q->endInsertRows();
}

void SchedulerModel::Private::testRemoved(const TestDefinitionPtr& test, int position)
{
    Q_UNUSED(test);

    q->beginRemoveRows(QModelIndex(), position, position);
    tests.removeAt(position);
    q->endRemoveRows();
}

void SchedulerModel::Private::testMoved(const TestDefinitionPtr& test, int from, int to)
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
        disconnect(d->scheduler.data(), SIGNAL(testAdded(TestDefinitionPtr,int)), d, SLOT(testAdded(TestDefinitionPtr,int)));
        disconnect(d->scheduler.data(), SIGNAL(testRemoved(TestDefinitionPtr,int)), d, SLOT(testRemoved(TestDefinitionPtr,int)));
        disconnect(d->scheduler.data(), SIGNAL(testMoved(TestDefinitionPtr,int,int)), d, SLOT(testMoved(TestDefinitionPtr,int,int)));
    }

    d->scheduler = scheduler;

    if (d->scheduler) {
        connect(d->scheduler.data(), SIGNAL(testAdded(TestDefinitionPtr,int)), d, SLOT(testAdded(TestDefinitionPtr,int)));
        connect(d->scheduler.data(), SIGNAL(testRemoved(TestDefinitionPtr,int)), d, SLOT(testRemoved(TestDefinitionPtr,int)));
        connect(d->scheduler.data(), SIGNAL(testMoved(TestDefinitionPtr,int,int)), d, SLOT(testMoved(TestDefinitionPtr,int,int)));
    }

    emit schedulerChanged();
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
    const TestDefinitionPtr& testDefinition = d->tests.at(index.row());

    if (role == Qt::DisplayRole)
        role = NameRole + index.column();

    switch(role) {
    case NameRole: return testDefinition->name();
    case NextRunRole: return testDefinition->timing()->nextRun();
    case TimeLeftRole: return testDefinition->timing()->timeLeft();
    case TypeRole: return testDefinition->timing()->type();
    case IdRole: return testDefinition->id();
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

#include "schedulermodel.moc"
