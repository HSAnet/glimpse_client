#ifndef ANDROIDPROCESSMODEL_H
#define ANDROIDPROCESSMODEL_H

#include <QAbstractListModel>

class AndroidProcessModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit AndroidProcessModel(QObject *parent = 0);
    ~AndroidProcessModel();

    enum Roles
    {
        PackageNameRole = Qt::UserRole + 1,
        DisplayNameRole
    };

    // QAbstractItemModel overrides
    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QHash<int, QByteArray> roleNames() const;

public slots:
    void reload();

protected:
    class AndroidProcessModelPrivate *d;
    friend class AndroidProcessModelPrivate;
};

#endif // ANDROIDPROCESSMODEL_H
