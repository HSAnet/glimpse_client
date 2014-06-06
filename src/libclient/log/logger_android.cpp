#include "logger.h"

#include <QAndroidJniObject>

void Logger::real_log(Logger::Level level, const QString &name, const QString &funcName, const QString &message)
{
    QByteArray logFuncName;

    switch (level)
    {
    case Trace:
        logFuncName = "i";
        break;

    case Debug:
        logFuncName = "d";
        break;

    case Info:
        logFuncName = "i";
        break;

    case Warning:
        logFuncName = "w";
        break;

    case Error:
        logFuncName = "e";
        break;
    }

    QString logMessage = QString("%1 : %2").arg(funcName).arg(message);

    QAndroidJniObject::callStaticMethod<jint>("android/util/Log",
                                              logFuncName.constData(),
                                              "(Ljava/lang/String;Ljava/lang/String;)I",
                                              QAndroidJniObject::fromString("mPlane").object<jstring>(),
                                              QAndroidJniObject::fromString(logMessage).object<jstring>());
}
