#ifndef LOGGER_H
#define LOGGER_H

#include "../export.h"

#include <QString>

// Compile-Time log levels
#define LEVEL_TRACE 0
#define LEVEL_DEBUG 1
#define LEVEL_INFO 2
#define LEVEL_WARNING 3
#define LEVEL_ERROR 4
#define LEVEL_NONE 5

#ifdef QT_DEBUG
#define LOG_LEVEL LEVEL_TRACE
#else
#define LOG_LEVEL LEVEL_INFO
#endif

class LogAppender;

class CLIENT_API Logger
{
public:
    Logger(const QString &name);
    ~Logger();

    enum Level
    {
        Trace,
        Debug,
        Info,
        Warning,
        Error
    };

    static void addAppender(LogAppender *appender);
    static void removeAppender(LogAppender *appender);

    void logTrace(const QString &funcName, const QString &message);
    void logDebug(const QString &funcName, const QString &message);
    void logInfo(const QString &funcName, const QString &message);
    void logWarning(const QString &funcName, const QString &message);
    void logError(const QString &funcName, const QString &message);

    void log(Level level, const QString &funcName, const QString &message);

private:
    void real_log(Level level, const QString &name, const QString &funcName, const QString &message);

private:
    QString m_name;
};

class LogAppender
{
public:
    virtual ~LogAppender() {}
    virtual void log(Logger::Level level, const QString &name, const QString &funcName, const QString &message) = 0;
};

#if LOG_LEVEL == LEVEL_NONE
#define LOGGER(x)
#else
#define LOGGER(x) \
    namespace { Logger logger(#x); }
#endif

#if LOG_LEVEL <= LEVEL_TRACE
#define LOG_TRACE(msg) logger.logTrace(Q_FUNC_INFO, msg);
#else
#define LOG_TRACE(msg) ;
#endif

#if LOG_LEVEL <= LEVEL_DEBUG
#define LOG_DEBUG(msg) logger.logDebug(Q_FUNC_INFO, msg);
#else
#define LOG_DEBUG(msg) ;
#endif

#if LOG_LEVEL <= LEVEL_INFO
#define LOG_INFO(msg) logger.logInfo(Q_FUNC_INFO, msg);
#else
#define LOG_INFO(msg) ;
#endif

#if LOG_LEVEL <= LEVEL_WARNING
#define LOG_WARNING(msg) logger.logWarning(Q_FUNC_INFO, msg);
#else
#define LOG_WARNING(msg) ;
#endif

#if LOG_LEVEL <= LEVEL_ERROR
#define LOG_ERROR(msg) logger.logError(Q_FUNC_INFO, msg);
#else
#define LOG_ERROR(msg) ;
#endif

#endif // LOGGER_H
