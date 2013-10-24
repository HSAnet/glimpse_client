#include "storagepaths.h"

class StoragePaths::Private
{
public:
};

StoragePaths::StoragePaths()
: d(new Private)
{
}

StoragePaths::~StoragePaths()
{
    delete d;
}

QDir StoragePaths::schedulerDirectory() const
{
}

QDir StoragePaths::reportDirectory() const
{
}

QDir StoragePaths::cacheDirectory() const
{
}
