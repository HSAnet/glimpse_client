#ifndef SCHEDULERSTORAGE_H
#define SCHEDULERSTORAGE_H

#include <QObject>

class Scheduler;

class SchedulerStorage : public QObject
{
    Q_OBJECT

public:
    SchedulerStorage(Scheduler* scheduler, QObject* parent = 0);
    ~SchedulerStorage();

    void setRealTime(bool realTime);
    bool isRealtime() const;

    void storeData();
    void loadData();

protected:
    class Private;
    Private* d;
};

#endif // SCHEDULERSTORAGE_H
