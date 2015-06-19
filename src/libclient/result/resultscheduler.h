#ifndef RESULTSCHEDULER_H
#define RESULTSCHEDULER_H

#include "result.h"

// ExtResultList includes the TaskId, and aggregated results with the same TaskId.
// It's basically a ReportList without all the unnecessary stuff.
typedef QList<QVariantMap> ExtResultList;

class CLIENT_API ResultScheduler : public QObject
{
    Q_OBJECT

public:
    ResultScheduler();
    ~ResultScheduler();

    ExtResultList results() const;

    void addResult(const TaskId &id, const Result &result);

signals:
    void resultAdded(const TaskId &id, const Result &result);
    void resultModified(const TaskId &id, const Result &result);

protected:
    class Private;
    Private *d;
};

#endif // RESULTSCHEDULER_H
