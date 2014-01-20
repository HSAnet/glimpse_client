#include "logmodel.h"
#include "../log/logger.h"
#include "../types.h"

#include <QList>

struct LogMessage
{
    Logger::Level level;
    QString name;
    QString message;
};

class LogModel::Private : public LogAppender
{
public:
    Private(LogModel* q)
    : q(q)
    {
        Logger::addAppender(this);
    }

    ~Private()
    {
        Logger::removeAppender(this);
    }

    LogModel* q;

    // Properties
    QList<LogMessage> lines;

    // LogAppender interface
    void log(Logger::Level level, const QString& name, const QString& funcName, const QString& message);
};

void LogModel::Private::log(Logger::Level level, const QString &name, const QString &funcName, const QString &message)
{
    LogMessage msg;
    msg.level = level;
    msg.name = name;
    msg.message = message;

    int pos = lines.size();
    q->beginInsertRows(QModelIndex(), pos, pos);
    lines.append(msg);
    q->endInsertRows();
}

LogModel::LogModel(QObject *parent)
: QAbstractListModel(parent)
, d(new Private(this))
{
}

LogModel::~LogModel()
{
    delete d;
}

int LogModel::rowCount(const QModelIndex &parent) const
{
    if ( parent.isValid() )
        return 0;

    return d->lines.size();
}

QVariant LogModel::data(const QModelIndex &index, int role) const
{
    int row = index.row();
    if (row < 0 || row >= d->lines.size())
        return QVariant();

    const LogMessage& msg = d->lines.at(row);

    switch (role) {
    case LevelRole: return msg.level;
    case NameRole: return msg.name;
    case MessageRole: return msg.message;

    default:
        break;
    }

    return QVariant();
}

QHash<int, QByteArray> LogModel::roleNames() const
{
    QHash<int, QByteArray> roleNames;
    roleNames.insert(LevelRole, "level");
    roleNames.insert(NameRole, "name");
    roleNames.insert(MessageRole, "message");
    return roleNames;
}
