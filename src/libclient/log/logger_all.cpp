#include "logger.h"

#include <QDebug>
#include <QTime>

void Logger::real_log(Logger::Level level, const QString& name, const QString &funcName, const QString &message)
{
    QString levelName;
    switch (level)
    {
    case Trace:
        levelName = "TRACE";
        break;
    case Debug:
        levelName = "DEBUG";
        break;
    case Info:
        levelName = "INFO";
        break;
    case Warning:
        levelName = "WARNING";
        break;
    case Error:
        levelName = "ERROR";
        break;
    }

    qDebug() << QTime::currentTime().toString() << levelName << funcName << ":" << message;
}
