#include "storage.h"

Storage::Storage()
{
}

bool Storage::deleteData()
{
    QList<QDir> dirs;
    dirs<<storagePath.cacheDirectory();
    dirs<<storagePath.schedulerDirectory();
    dirs<<storagePath.reportDirectory();

    foreach (QDir dir, dirs)
    {
        dir.setFilter(QDir::NoDotAndDotDot | QDir::Files);
        foreach (const QString &dirItem, dir.entryList())
        {
            dir.remove(dirItem);
        }
    }

    return true;
}
