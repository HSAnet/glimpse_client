#ifndef REPORTCONTROLLER_H
#define REPORTCONTROLLER_H

#include "controller.h"

class Settings;
class ReportScheduler;

class CLIENT_API ReportController : public Controller
{
    Q_OBJECT

public:
    explicit ReportController(QObject* parent = 0);
    ~ReportController();

    // Controller interface
    Status status() const;
    QString errorString() const;

    bool init(ReportScheduler* scheduler, Settings* settings);

public slots:
    void sendReports();

protected:
    class Private;
    Private* d;
};

#endif // REPORTCONTROLLER_H
