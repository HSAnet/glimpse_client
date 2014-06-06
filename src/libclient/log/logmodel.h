#ifndef LOGMODEL_H
#define LOGMODEL_H

#include "../export.h"

#include <QAbstractListModel>

class CLIENT_API LogModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit LogModel(QObject *parent = 0);
    ~LogModel();

    enum Roles
    {
        LevelRole = Qt::UserRole + 1,
        NameRole,
        MessageRole
    };

    // QAbstractItemModel interface
    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QHash<int, QByteArray> roleNames() const;

protected:
    class Private;
    Private *d;
};

#endif // LOGMODEL_H
