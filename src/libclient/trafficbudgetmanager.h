#ifndef TRAFFIC_BUDGET_MANAGER_H
#define TRAFFIC_BUDGET_MANAGER_H

#include <QObject>

#include "export.h"

class CLIENT_API TrafficBudgetManager : public QObject
{
    Q_OBJECT

public:
    explicit TrafficBudgetManager(QObject *parent = 0);
    ~TrafficBudgetManager();

    void init();
    void saveTraffic();

    quint32 availableTraffic() const;

    bool addUsedTraffic(quint32 traffic);
    quint32 usedTraffic() const;

    void reset();

protected:
    class Private;
    Private *d;
};

#endif
