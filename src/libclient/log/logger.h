#ifndef LOGGER_H
#define LOGGER_H

#include "../export.h"

#include <QString>

// Compile-Time log levels
#define LEVEL_TRACE 0
#define LEVEL_INFO 1
#define LEVEL_WARNING 2
#define LEVEL_ERROR 3

#define LOG_LEVEL LEVEL_TRACE

class CLIENT_API Logger
{
public:
    Logger(const QString& name);
    ~Logger();

    enum Level
    {
        Trace,
        Info,
        Warning,
        Error
    };

    void logTrace(const QString& message);
    void logInfo(const QString& message);
    void logWarning(const QString& message);
    void logError(const QString& message);

    void log(Level level, const QString& message);

private:
    QString m_name;
};

#define LOGGER(x) \
    namespace { Logger log(#x); }

#if LOG_LEVEL <= LEVEL_TRACE
#define LOG_TRACE(msg) log.logTrace(msg);
#else
#define LOG_TRACE(msg)
#endif

#if LOG_LEVEL <= LEVEL_INFO
#define LOG_INFO(msg) log.logInfo(msg);
#else
#define LOG_INFO(msg)
#endif

#if LOG_LEVEL <= LEVEL_WARNING
#define LOG_WARNING(msg) log.logWarning(msg);
#else
#define LOG_WARNING(msg)
#endif

#if LOG_LEVEL <= LEVEL_ERROR
#define LOG_ERROR(msg) log.logError(msg);
#else
#define LOG_ERROR(msg)
#endif

#endif // LOGGER_H
