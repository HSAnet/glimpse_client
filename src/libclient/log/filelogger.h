#ifndef FILELOGGER_H
#define FILELOGGER_H

#include "../log/logger.h"

#include <QDir>
#include <QFile>
#include <QTextStream>

class FileLogger : public LogAppender
{
public:
    FileLogger();
    ~FileLogger();

    // LogAppender interface
    void log(Logger::Level level, const QString& name, const QString& funcName, const QString& message);

private:
    // Properties
    QDir dir;
    QFile outFile;
    QTextStream out;
};

#endif // FILELOGGER_H
