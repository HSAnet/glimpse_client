#ifndef STORAGE_H
#define STORAGE_H

#include "storagepaths.h"

class Storage
{
public:
    Storage();

    bool deleteData();

private:
    StoragePaths storagePath;
};

#endif // STORAGE_H
