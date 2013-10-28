#ifndef MACPROCESSMODEL_H
#define MACPROCESSMODEL_H

#include <QAbstractListModel>

class MacProcessModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit MacProcessModel(QObject* parent = 0);
    ~MacProcessModel();

    enum Roles
    {
        DisplayNameRole = Qt::UserRole +1
    };

    // QAbstractItemModel overrides
    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QHash<int, QByteArray> roleNames() const;

public slots:
    void reload();

protected:
    class Private;
    Private* d;
};

#endif // MACPROCESSMODEL_H
