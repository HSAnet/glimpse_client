#import "storagepaths.h"
#include "../log/logger.h"

#include <QCoreApplication>
#include <QDir>

LOGGER(StoragePaths);

class StoragePaths::Private
{
public:
    QString applicationRootPath;
};

StoragePaths::StoragePaths()
: d(new Private)
{
    QDir dir(QCoreApplication::applicationDirPath());
    dir.cdUp();
    dir.cd("Documents");

    d->applicationRootPath = dir.absolutePath();
}

StoragePaths::~StoragePaths()
{
    delete d;
}

QDir StoragePaths::schedulerDirectory() const
{
    return QDir(d->applicationRootPath + "/scheduler");
}

QDir StoragePaths::reportDirectory() const
{
    return QDir(d->applicationRootPath + "/reports");
}

QDir StoragePaths::cacheDirectory() const
{
    QDir dir(QCoreApplication::applicationDirPath());
    dir.cdUp();
    dir.cd("Library");
    dir.cd("Caches");
    dir.cd("mPlane");
    return dir;
}

QDir StoragePaths::crashDumpDirectory() const
{
    return QDir(d->applicationRootPath + "/crashdumps");
}
