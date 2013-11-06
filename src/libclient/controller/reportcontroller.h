#ifndef REPORTCONTROLLER_H
#define REPORTCONTROLLER_H

#include "../export.h"

#include <QObject>

class Settings;
class ReportScheduler;

class CLIENT_API ReportController : public QObject
{
    Q_OBJECT
    Q_ENUMS(Status)
    Q_PROPERTY(Status status READ status NOTIFY statusChanged)

public:
    explicit ReportController(QObject* parent = 0);
    ~ReportController();

    enum Status {
        Unknown,
        Running,
        Finished,
        Error
    };

    Status status() const;

    bool init(ReportScheduler* scheduler, Settings* settings);

    Q_INVOKABLE QString errorString() const;
    Q_INVOKABLE void sendReports();

signals:
    void statusChanged();

protected:
    class Private;
    Private* d;
};

#endif // REPORTCONTROLLER_H
