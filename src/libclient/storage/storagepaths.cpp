#include "storagepaths.h"

#include <QStandardPaths>

StoragePaths::StoragePaths()
: d(NULL)
{
}

StoragePaths::~StoragePaths()
{
}

QDir StoragePaths::schedulerDirectory() const
{
    return QDir(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/scheduler");
}

QDir StoragePaths::reportDirectory() const
{
    return QDir(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/reports");
}

QDir StoragePaths::cacheDirectory() const
{
    return QDir(QStandardPaths::writableLocation(QStandardPaths::CacheLocation));
}

QDir StoragePaths::logDirectory() const
{
    return QDir(QStandardPaths::writableLocation(QStandardPaths::DataLocation));
}

QDir StoragePaths::crashDumpDirectory() const
{
    return QDir(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/crashdumps");
}
