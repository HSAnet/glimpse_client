#ifndef REPORTSTORAGE_H
#define REPORTSTORAGE_H

#include "reportscheduler.h"

class CLIENT_API ReportStorage : public QObject
{
    Q_OBJECT

public:
    ReportStorage(ReportScheduler* scheduler, QObject* parent = 0);
    ~ReportStorage();

    void setRealTime(bool realTime);
    bool isRealTime() const;

    void storeData();
    void loadData();

protected:
    class Private;
    Private* d;
};


#endif // REPORTSTORAGE_H
