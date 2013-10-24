#ifndef REPORTMODEL_H
#define REPORTMODEL_H

#include "../export.h"

#include <QAbstractTableModel>

class ReportScheduler;

class CLIENT_API ReportModel : public QAbstractTableModel
{
    Q_OBJECT
    Q_ENUMS(Roles)
    Q_PROPERTY(ReportScheduler* scheduler READ scheduler WRITE setScheduler NOTIFY schedulerChanged)

public:
    ReportModel(QObject* parent = 0);
    ~ReportModel();

    enum Roles
    {
        TaskIdRole = Qt::UserRole + 1,
        DateTimeRole,
        ResultsRole
    };

    void setScheduler(ReportScheduler* scheduler);
    ReportScheduler* scheduler() const;

    Q_INVOKABLE void reset();
    Q_INVOKABLE QVariant get(int index) const;

    // QAbstractTableModel overrides
    QHash<int, QByteArray> roleNames() const;

    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

signals:
    void schedulerChanged();

protected:
    class Private;
    Private* d;
};

#endif // REPORTMODEL_H
