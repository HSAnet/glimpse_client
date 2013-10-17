#include "logger.h"
#include "../types.h"

#include <QDebug>

Logger::Logger(const QString &name)
: m_name(name)
{
}

Logger::~Logger()
{
}

void Logger::logTrace(const QString &message)
{
    log(Trace, message);
}

void Logger::logInfo(const QString &message)
{
    log(Info, message);
}

void Logger::logWarning(const QString &message)
{
    log(Warning, message);
}

void Logger::logError(const QString &message)
{
    log(Error, message);
}

void Logger::log(Logger::Level level, const QString &message)
{
    QString levelName;
    switch(level) {
    case Trace: levelName = "TRACE"; break;
    case Info: levelName = "INFO"; break;
    case Warning: levelName = "WARNING"; break;
    case Error: levelName = "ERROR"; break;
    }

    qDebug() << levelName << m_name << ":" << message;
}
