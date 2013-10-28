#include "logger.h"

#include <QAndroidJniObject>

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
    QByteArray logFuncName;
    switch(level) {
    case Trace: logFuncName = "i"; break;
    case Debug: logFuncName = "d"; break;
    case Info: logFuncName = "i"; break;
    case Warning: logFuncName = "w"; break;
    case Error: logFuncName = "e"; break;
    }

    QString logMessage = QString("%1 : %2").arg(funcName).arg(message);

    QAndroidJniObject::callStaticMethod<jint>("android/util/Log",
                                              logFuncName.constData(),
                                              "(Ljava/lang/String;Ljava/lang/String;)I",
                                              QAndroidJniObject::fromString("mPlane").object<jstring>(),
                                              QAndroidJniObject::fromString(logMessage).object<jstring>());
}

