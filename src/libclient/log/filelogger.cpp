#include "filelogger.h"
#include <QDateTime>
#include <QFileInfo>
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

    if (rotate())
    {
        out << QDateTime::currentDateTime().toString() << " " << levelName << " " << funcName << " : " << message << endl;
        out.flush();
    }
}

bool FileLogger::rotate(quint32 backlog)
{
    QFileInfo info(outFile);
    QDir dir(info.absoluteDir());

    if (info.exists() && info.lastModified().date() < QDateTime::currentDateTime().date())
    {
        outFile.close();

        for (quint32 i = backlog - 1; i > 0; i--)
        {
            QString oldName(QString("glimpse.log.%1").arg(QString::number(i)));
            QString newName(QString("glimpse.log.%1").arg(QString::number(i + 1)));

            if (dir.exists(oldName))
            {
                dir.remove(newName);
                dir.rename(oldName, newName);
            }
        }

        QString newName(QString("glimpse.log.%1").arg(QString::number(1)));
        dir.rename("glimpse.log", newName);

        if (!outFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
        {
            return false;
        }
    }

    return true;
}
