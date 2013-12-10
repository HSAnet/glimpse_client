#ifndef LINUXPROCESSMODEL_H
#define LINUXPROCESSMODEL_H

#include <QAbstractListModel>

class LinuxProcessModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit LinuxProcessModel(QObject* parent = 0);
    ~LinuxProcessModel();

    enum Roles
    {
        DisplayNameRole = Qt::UserRole +1,
        BundleName
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

#endif // LINUXPROCESSMODEL_H
