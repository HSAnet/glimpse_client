#ifndef TASKSTORAGE_H
#define TASKSTORAGE_H

#include "../scheduler/scheduler.h"

class CLIENT_API TaskStorage : public QObject
{
    Q_OBJECT
public:
    explicit TaskStorage(Scheduler *scheduler, QObject *parent = 0);
    ~TaskStorage();

    void storeData();
    void loadData();

protected:
    class Private;
    Private *d;
};

#endif // TASKSTORAGE_H
