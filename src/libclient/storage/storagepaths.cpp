#include "storagepaths.h"

#include <QStandardPaths>

StoragePaths::StoragePaths()
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
