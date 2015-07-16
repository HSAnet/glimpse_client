#ifndef RESULTCONTROLLER_H
#define RESULTCONTROLLER_H

#include "controller.h"
#include "../result/resultscheduler.h"

class Settings;

class CLIENT_API ResultController : public Controller
{
    Q_OBJECT

public:
    ResultController(QObject *parent = 0);
    ~ResultController();

    bool init(ResultScheduler *scheduler, Settings *settings);

    // Controller interface
    Status status() const;
    QString errorString() const;

protected:
    class Private;
    Private *d;
};

#endif // RESULTCONTROLLER_H
