#ifndef WINDOWSPROCESSMODEL_H
#define WINDOWSPROCESSMODEL_H

class WindowsProcessModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit WindowsProcessModel(QObject* parent = 0);
    ~WindowsProcessModel();

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

#endif // WINDOWSPROCESSMODEL_H
