#ifndef REPORTCONTROLLER_H
#define REPORTCONTROLLER_H

#include "../export.h"

#include <QObject>

class Settings;
class ReportScheduler;

class CLIENT_API ReportController : public QObject
{
    Q_OBJECT

public:
    explicit ReportController(QObject* parent = 0);
    ~ReportController();

    bool init(ReportScheduler* scheduler, Settings* settings);

    Q_INVOKABLE void sendReports();

protected:
    class Private;
    Private* d;
};

#endif // REPORTCONTROLLER_H
