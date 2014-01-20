#include "linuxprocessmodel.h"

#include <QStringList>
#include <QByteArray>
#include <QProcess>
#include <QTextStream>
#include <QRegExp>
#include <QFile>

struct ProcessInfo
{
    QString localizedName;
    QString bundleName;
};

class LinuxProcessModel::Private : public QObject
{
    Q_OBJECT

public:
    QList<ProcessInfo> apps;

    Private(LinuxProcessModel* q)
    : q(q)
    , processPattern("[ ]*(\\d+) (.*)$") // " 3862 thunderbird"
    , stream(&process)
    {
        connect(&process, SIGNAL(readyRead()), this, SLOT(onReadyRead()));

        interpreters << "python" << "perl" << "bash" << "ruby";
    }

    LinuxProcessModel* q;

    // Properties
    QStringList interpreters;

    QRegExp processPattern;
    QProcess process;
    QTextStream stream;
    QStringList processes;

    // Functions
    QString getFullCommand(const QString &pid) const;

public slots:
    void onReadyRead();
};

LinuxProcessModel::LinuxProcessModel(QObject *parent)
: QAbstractListModel(parent)
, d(new Private(this))
{
    reload();
}

LinuxProcessModel::~LinuxProcessModel()
{
    delete d;
}

int LinuxProcessModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return d->apps.size();
}

QVariant LinuxProcessModel::data(const QModelIndex &index, int role) const
{
    const ProcessInfo& app = d->apps.at(index.row());

    switch (role) {
    case DisplayNameRole: return app.localizedName;
    case BundleName: return app.bundleName;

    default:
        break;
    }

    return QVariant();
}

QHash<int, QByteArray> LinuxProcessModel::roleNames() const
{
    QHash<int, QByteArray> roleNames;
    roleNames.insert(DisplayNameRole, "displayName");
    roleNames.insert(BundleName, "packageName");
    return roleNames;
}

void LinuxProcessModel::reload()
{
    beginResetModel();
    d->apps.clear();

    QStringList arguments;
    arguments << "xo" << "pid,comm";

    d->process.start("ps", arguments);
    d->process.waitForFinished();

    endResetModel();
}

QString LinuxProcessModel::Private::getFullCommand(const QString& pid) const
{
    QString fileName = QString("/proc/%1/cmdline").arg(pid);

    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly)) {
        QByteArray link = file.readAll();

        foreach(const QString& interpreter, interpreters) {
            QString pattern = QString("/bin/%1").arg(interpreter);
            if ( link.contains(pattern.toLatin1()) ) {
                link = link.split('\0').at(1);
                break;
            }
        }

        return link.split(' ').at(0);
    }

    return QString();
}

void LinuxProcessModel::Private::onReadyRead()
{
    QString line = stream.readLine();
    while (!line.isEmpty()) {
        int pos = processPattern.indexIn(line);
        if ( pos != -1 ) {
            // Zombie processes might return nothing
            QString commandline = getFullCommand(processPattern.cap(1));
            if ( !commandline.isEmpty() ) {
                ProcessInfo psInfo;
                psInfo.bundleName = processPattern.cap(1);
                psInfo.localizedName = processPattern.cap(2);
                apps.append( psInfo );
            }
        }
        line = stream.readLine();
    }
}

#include "linuxprocessmodel.moc"
