#ifndef STORAGEPATHS_H
#define STORAGEPATHS_H

#include "../export.h"

#include <QDir>

class CLIENT_API StoragePaths
{
public:
    StoragePaths();
    ~StoragePaths();

    QDir schedulerDirectory() const;
    QDir reportDirectory() const;
    QDir cacheDirectory() const;
    QDir logDirectory() const;

    QDir crashDumpDirectory() const;

protected:
    class Private;
    Private *d;
};

#endif // STORAGEPATHS_H
