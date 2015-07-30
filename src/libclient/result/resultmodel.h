#ifndef RESULTMODEL_H
#define RESULTMODEL_H

#include "../export.h"
#include "../ident.h"

#include <QAbstractTableModel>

class ResultScheduler;

class CLIENT_API ResultModel : public QAbstractTableModel
{
    Q_OBJECT
    Q_ENUMS(Roles)
    Q_PROPERTY(ResultScheduler *scheduler READ scheduler WRITE setScheduler NOTIFY schedulerChanged)

public:
    ResultModel(QObject *parent = 0);
    ~ResultModel();

    enum Roles {
        TaskIdRole = Qt::UserRole + 1,
        DateTimeRole,
        ResultsRole,
        MeasurementRole,
        SecondColumnRole
    };

    void setScheduler(ResultScheduler *scheduler);
    ResultScheduler *scheduler() const;

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
    Private *d;
};

#endif // RESULTMODEL_H
