#ifndef SCHEDULERMODEL_H
#define SCHEDULERMODEL_H

#include <QAbstractTableModel>

class Scheduler;

class SchedulerModel : public QAbstractTableModel
{
    Q_ENUMS(Roles Mode)
    Q_OBJECT

public:
    SchedulerModel(QObject* parent = 0);
    ~SchedulerModel();

    enum Roles
    {
        NameRole = Qt::UserRole + 1,
        IdRole,
        NextRunRole,
        TimeLeftRole,
        TypeRole
    };

    enum Mode
    {
    };

    void setScheduler(Scheduler* scheduler);
    Scheduler* scheduler() const;

    Q_INVOKABLE void reset();
    Q_INVOKABLE QVariant get(int index) const;

    // QAbstractTableModel overrides
    QHash<int, QByteArray> roleNames() const;

    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

protected:
    class Private;
    Private* d;
};

#endif // SCHEDULERMODEL_H
