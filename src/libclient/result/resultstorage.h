#ifndef RESULTSTORAGE_H
#define RESULTSTORAGE_H

#include "resultscheduler.h"
#include "report/reportscheduler.h"

class CLIENT_API ResultStorage : public QObject
{
    Q_OBJECT

public:
    ResultStorage(ResultScheduler *scheduler, ReportScheduler *reportScheduler, QObject *parent = 0);
    ~ResultStorage();

    void init();

    void storeData();
    void loadData();

protected:
    class Private;
    Private *d;
};

#endif // RESULTSTORAGE_H
