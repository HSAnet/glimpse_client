#ifndef SCHEDULERSTORAGE_H
#define SCHEDULERSTORAGE_H

#include "export.h"

#include <QObject>

class Scheduler;

class CLIENT_API SchedulerStorage : public QObject
{
    Q_OBJECT

public:
    SchedulerStorage(Scheduler *scheduler, QObject *parent = 0);
    ~SchedulerStorage();

    void storeData();
    void loadData();

protected:
    class Private;
    Private *d;
};

#endif // SCHEDULERSTORAGE_H
