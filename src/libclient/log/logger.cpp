#include "logger.h"
#include "../types.h"

#include <QList>
#include <QMutexLocker>

typedef QList<LogAppender*> LogAppenderList;

namespace
{
QMutex mutex;
}

LogAppenderList& appenders()
{
    static LogAppenderList appenders;
    return appenders;
}

Logger::Logger(const QString &name)
: m_name(name)
{
}

Logger::~Logger()
{
}

void Logger::addAppender(LogAppender *appender)
{
    appenders().append(appender);
}

void Logger::removeAppender(LogAppender *appender)
{
    appenders().removeAll(appender);
}

void Logger::logTrace(const QString &funcName, const QString &message)
{
    log(Trace, funcName, message);
}

void Logger::logDebug(const QString &funcName, const QString &message)
{
    log(Debug, funcName, message);
}

void Logger::logInfo(const QString &funcName, const QString &message)
{
    log(Info, funcName, message);
}

void Logger::logWarning(const QString &funcName, const QString &message)
{
    log(Warning, funcName, message);
}

void Logger::logError(const QString &funcName, const QString &message)
{
    log(Error, funcName, message);
}

void Logger::log(Logger::Level level, const QString &funcName, const QString &message)
{
    QMutexLocker locker(&mutex);

    real_log(level, m_name, funcName, message);

    foreach(LogAppender* appender, appenders())
    {
        appender->log(level, m_name, funcName, message);
    }
}
