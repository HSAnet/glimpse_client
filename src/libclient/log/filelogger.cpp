#include "filelogger.h"
#include <QTime>
#include <storage/storagepaths.h>

LOGGER(FileLogger);

FileLogger::FileLogger()
    : dir(StoragePaths().logDirectory())
    , out(&outFile)
{
    if (!dir.exists())
    {
        if (!QDir::root().mkpath(dir.absolutePath()))
        {
            LOG_ERROR(QString("Unable to create path %1").arg(dir.absolutePath()));
            return;
        }
        else
        {
            LOG_INFO("Log directory created");
        }
    }

    outFile.setFileName(dir.absoluteFilePath(("glimpse.log")));

    if (!outFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
    {
        return;
    }
}

FileLogger::~FileLogger()
{
    Logger::removeAppender(this);
    outFile.close();
}

void FileLogger::log(Logger::Level level, const QString &name, const QString &funcName, const QString &message)
{
    Q_UNUSED(name);

    QString levelName;
    switch (level)
    {
    case Logger::Trace:
        levelName = "TRACE";
        break;
    case Logger::Debug:
        levelName = "DEBUG";
        break;
    case Logger::Info:
        levelName = "INFO";
        break;
    case Logger::Warning:
        levelName = "WARNING";
        break;
    case Logger::Error:
        levelName = "ERROR";
        break;
    }

    out << QTime::currentTime().toString() << " " << levelName << " " << funcName << " : " << message << endl;
}
