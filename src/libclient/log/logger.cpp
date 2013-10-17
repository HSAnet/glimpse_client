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

void Logger::logTrace(const QString &funcName, const QString &message)
{
    log(Trace, funcName, message);
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
    QString levelName;
    switch(level) {
    case Trace: levelName = "TRACE"; break;
    case Info: levelName = "INFO"; break;
    case Warning: levelName = "WARNING"; break;
    case Error: levelName = "ERROR"; break;
    }

    qDebug() << levelName << funcName << m_name << ":" << message;
}
