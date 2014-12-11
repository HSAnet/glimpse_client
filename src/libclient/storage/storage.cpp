#include "storage.h"

Storage::Storage()
{
}

bool Storage::deleteData()
{
    storagePath.logDirectory().removeRecursively();
    storagePath.cacheDirectory().removeRecursively();
    storagePath.schedulerDirectory().removeRecursively();
    storagePath.crashDumpDirectory().removeRecursively();
    storagePath.reportDirectory().removeRecursively();

    return true;
}
