#include "windowsprocessmodel.h"

struct ProcessInfo
{
    QString localizedName;
    QString bundleName;
};

class WindowsProcessModel::Private : public QObject
{
    Q_OBJECT

public:
    QList<ProcessInfo> apps;

    Private(WindowsProcessModel* q)
    : q(q)
    {
        connect(&process, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
    }

    WindowsProcessModel* q;

    // Properties
    QStringList interpreters;

    QProcess process;
    QTextStream stream;

    // Functions
    QString getFullCommand(const QString &pid) const;

public slots:
    void onReadyRead();
};

WindowsProcessModel::WindowsProcessModel(QObject *parent)
: QAbstractListModel(parent)
, d(new Private(this))
{
    reload();
}

WindowsProcessModel::~WindowsProcessModel()
{
    delete d;
}

int WindowsProcessModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return d->apps.size();
}

QVariant WindowsProcessModel::data(const QModelIndex &index, int role) const
{
    const ProcessInfo& app = d->apps.at(index.row());

    switch(role) {
    case DisplayNameRole: return app.localizedName;
    case BundleName: return app.bundleName;

    default:
        break;
    }

    return QVariant();
}

QHash<int, QByteArray> WindowsProcessModel::roleNames() const
{
    QHash<int, QByteArray> roleNames;
    roleNames.insert(DisplayNameRole, "displayName");
    roleNames.insert(BundleName, "packageName");
    return roleNames;
}

void WindowsProcessModel::reload()
{
    beginResetModel();
    d->apps.clear();

    QStringList arguments;
    arguments << "xo" << "pid,comm";

    d->process.start("ps", arguments);
    d->process.waitForFinished();

    endResetModel();
}

QString WindowsProcessModel::Private::getFullCommand(const QString& pid) const
{
    return QString();
}

void WindowsProcessModel::Private::onReadyRead()
{
}

#include "windowsprocessmodel.moc"
