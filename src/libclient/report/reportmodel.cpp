#include "reportmodel.h"
#include "reportscheduler.h"
#include "../types.h"
#include "task/scheduledefinition.h"
#include "scheduler/scheduler.h"
#include "client.h"

#include <QPointer>
#include <QUuid>
#include <QHash>

class ReportModel::Private : public QObject
{
    Q_OBJECT

public:
    Private(ReportModel *q)
    : q(q)
    {
    }

    ReportModel *q;

    QPointer<ReportScheduler> scheduler;

    QHash<TaskId, int> identToReport;
    ReportList reports;

public slots:
    void reportAdded(const Report &report);
    void reportModified(const Report &report);
    void reportRemoved(const Report &report);
};

void ReportModel::Private::reportAdded(const Report &report)
{
    int position = reports.size();

    q->beginInsertRows(QModelIndex(), position, position);
    identToReport.insert(report.taskId(), position);
    reports.append(report);
    q->endInsertRows();
}

void ReportModel::Private::reportModified(const Report &report)
{
    int pos = identToReport.value(report.taskId());
    Q_ASSERT(pos != -1);

    // No need to change identToReport here
    reports.replace(pos, report);

    QModelIndex index = q->indexFromTaskId(report.taskId());
    Q_ASSERT(index.isValid());
    emit q->dataChanged(index, index);
}

void ReportModel::Private::reportRemoved(const Report &report)
{
    int position = identToReport.value(report.taskId());
    Q_ASSERT(position != -1);

    q->beginRemoveRows(QModelIndex(), position, position);
    identToReport.remove(report.taskId());
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
    {
        return;
    }

    if (d->scheduler)
    {
        disconnect(d->scheduler.data(), SIGNAL(reportAdded(Report)), d, SLOT(reportAdded(Report)));
        disconnect(d->scheduler.data(), SIGNAL(reportModified(Report)), d, SLOT(reportModified(Report)));
        disconnect(d->scheduler.data(), SIGNAL(reportRemoved(Report)), d, SLOT(reportRemoved(Report)));
    }

    d->scheduler = scheduler;

    if (d->scheduler)
    {
        connect(d->scheduler.data(), SIGNAL(reportAdded(Report)), d, SLOT(reportAdded(Report)));
        connect(d->scheduler.data(), SIGNAL(reportModified(Report)), d, SLOT(reportModified(Report)));
        connect(d->scheduler.data(), SIGNAL(reportRemoved(Report)), d, SLOT(reportRemoved(Report)));
    }

    emit schedulerChanged();
    reset();
}

ReportScheduler *ReportModel::scheduler() const
{
    return d->scheduler;
}

QModelIndex ReportModel::indexFromTaskId(const TaskId &taskId) const
{
    int pos = d->identToReport.value(taskId);

    if (pos == -1)
    {
        return QModelIndex();
    }
    else
    {
        return createIndex(pos, 0);
    }
}

void ReportModel::reset()
{
    beginResetModel();

    if (!d->scheduler.isNull())
    {
        d->reports = d->scheduler->reports();
    }

    endResetModel();
}

QVariant ReportModel::get(int index) const
{
    if (index < 0 || index >= d->reports.size())
    {
        return QVariant();
    }

    const Report &report = d->reports.at(index);
    return report.toVariant();
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
    {
        return 0;
    }

    return d->reports.size();
}

int ReportModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 5;
}

QVariant ReportModel::data(const QModelIndex &index, int role) const
{
    const Report &report = d->reports.at(index.row());

    if (role == Qt::DisplayRole)
    {
        role = TaskIdRole + index.column();
    }

    switch (role)
    {
    case TaskIdRole:
        return report.taskId().toInt();

    case DateTimeRole:
        return report.dateTime();
        //case ResultsRole: return report->results(); // TODO: Scripting can't do anything with that
    }

    return QVariant();
}

QVariant ReportModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole || orientation != Qt::Horizontal)
    {
        return QVariant();
    }

    section += TaskIdRole;

    switch (section)
    {
    case TaskIdRole:
        return tr("Task Id");

    case DateTimeRole:
        return tr("Date-Time");

    case ResultsRole:
        return tr("Results");

    default:
        return QVariant();
    }
}

#include "reportmodel.moc"
