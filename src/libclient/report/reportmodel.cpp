#include "reportmodel.h"
#include "reportscheduler.h"

#include <QPointer>
#include <QUuid>

class ReportModel::Private : public QObject
{
    Q_OBJECT

public:
    Private(ReportModel* q)
    : q(q)
    {
    }

    ReportModel* q;

    QPointer<ReportScheduler> scheduler;
    ReportList reports;

public slots:
    void reportAdded(const ReportPtr &report);
    void reportModified(const ReportPtr &report);
    void reportRemoved(const ReportPtr &report);
};

void ReportModel::Private::reportAdded(const ReportPtr& report)
{
    int position = reports.size();

    q->beginInsertRows(QModelIndex(), position, position);
    reports.append(report);
    q->endInsertRows();
}

void ReportModel::Private::reportModified(const ReportPtr& report)
{
    // TODO: Move entries
    /*
    q->beginMoveRows(QModelIndex(), from, from,
                     QModelIndex(), to+1);
    reports.move(from, to);
    q->endMoveRows();
    */
}

void ReportModel::Private::reportRemoved(const ReportPtr& report)
{
    Q_UNUSED(report);

    int position = reports.indexOf(report);
    Q_ASSERT(position != -1);

    q->beginRemoveRows(QModelIndex(), position, position);
    reports.removeAt(position);
    q->endRemoveRows();
}

ReportModel::ReportModel(QObject *parent)
: QAbstractTableModel(parent)
, d(new Private(this))
{
}

ReportModel::~ReportModel()
{
    delete d;
}

void ReportModel::setScheduler(ReportScheduler *scheduler)
{
    if (d->scheduler == scheduler)
        return;

    if (d->scheduler) {
        disconnect(d->scheduler.data(), SIGNAL(reportAdded(ReportPtr)), d, SLOT(reportAdded(ReportPtr)));
        disconnect(d->scheduler.data(), SIGNAL(reportModified(ReportPtr)), d, SLOT(reportModified(ReportPtr)));
        disconnect(d->scheduler.data(), SIGNAL(reportRemoved(ReportPtr)), d, SLOT(reportRemoved(ReportPtr)));
    }

    d->scheduler = scheduler;

    if (d->scheduler) {
        connect(d->scheduler.data(), SIGNAL(reportAdded(ReportPtr)), d, SLOT(reportAdded(ReportPtr)));
        connect(d->scheduler.data(), SIGNAL(reportModified(ReportPtr)), d, SLOT(reportModified(ReportPtr)));
        connect(d->scheduler.data(), SIGNAL(reportRemoved(ReportPtr)), d, SLOT(reportRemoved(ReportPtr)));
    }

    emit schedulerChanged();
    reset();
}

ReportScheduler *ReportModel::scheduler() const
{
    return d->scheduler;
}

void ReportModel::reset()
{
    beginResetModel();

    if (!d->scheduler.isNull())
        d->reports = d->scheduler->reports();

    endResetModel();
}

QVariant ReportModel::get(int index) const
{
    if (index < 0 || index >= d->reports.size())
        return QVariant();

    const ReportPtr& report = d->reports.at(index);
    return report->toVariant();
}

QHash<int, QByteArray> ReportModel::roleNames() const
{
    QHash<int, QByteArray> roleNames;
    roleNames.insert(TaskIdRole, "taskId");
    roleNames.insert(DateTimeRole, "dateTime");
    roleNames.insert(ResultsRole, "results");
    return roleNames;
}

int ReportModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return d->reports.size();
}

int ReportModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 5;
}

QVariant ReportModel::data(const QModelIndex &index, int role) const
{
    const ReportPtr& report = d->reports.at(index.row());

    if (role == Qt::DisplayRole)
        role = TaskIdRole + index.column();

    switch(role) {
    case TaskIdRole: return report->taskId();
    case DateTimeRole: return report->dateTime();
    //case ResultsRole: return report->results(); // FIXME: Scripting can't do anything with that
    }

    return QVariant();
}

QVariant ReportModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole || orientation != Qt::Horizontal)
        return QVariant();

    section += TaskIdRole;

    switch(section) {
    case TaskIdRole: return tr("Task Id");
    case DateTimeRole: return tr("Date-Time");
    case ResultsRole: return tr("Results");

    default:
        return QVariant();
    }
}

#include "reportmodel.moc"
