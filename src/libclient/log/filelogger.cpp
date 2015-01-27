#include "filelogger.h"
#include <QDateTime>
#include <QFileInfo>
#include <storage/storagepaths.h>

LOGGER(FileLogger);

FileLogger::FileLogger(quint32 backlog)
: dir(StoragePaths().logDirectory())
, out(&outFile)
, m_backlog(backlog)
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

    if (!rotate() || (!outFile.isOpen() && !outFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)))
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

bool FileLogger::rotate()
{
    QFileInfo info(outFile);
    QDir dir(info.absoluteDir());

    if (m_backlog == 0)
    {
        return true;
    }

    if (info.exists() && info.lastModified().date() < QDateTime::currentDateTime().date())
    {
        outFile.close();

        for (quint32 i = m_backlog - 1; i > 0; i--)
        {
            QString oldName(QString("glimpse.log.%1").arg(QString::number(i)));
            QString newName(QString("glimpse.log.%1").arg(QString::number(i + 1)));

            if (dir.exists(oldName))
            {
                dir.remove(newName);
                dir.rename(oldName, newName);
            }
        }

        dir.rename("glimpse.log", "glimpse.log.1");

        if (!outFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append))
        {
            return false;
        }
    }

    return true;
}
