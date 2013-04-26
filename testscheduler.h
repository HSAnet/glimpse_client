#ifndef TESTSCHEDULER_H
#define TESTSCHEDULER_H

#include "tests/test.h"
#include "tests/testinfo.h"
#include "types.h"

#include <QObject>
#include <QUuid>
#include <QUdpSocket>

class TestScheduler : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool isStarted READ isStarted NOTIFY isStartedChanged)
    Q_PROPERTY(AbstractTest* currentTest READ currentTest NOTIFY currentTestChanged)

public:
    explicit TestScheduler(QObject *parent = 0);
    ~TestScheduler();

    QUdpSocket* managerSocket() const;
    void setManagerSocket(QUdpSocket* managerSocket);

    // TODO: Functions
    void enqueue(const TestInfo& info);
    //void dequeue();

    bool isStarted() const;

    AbstractTest* currentTest() const;

public slots:
    void start();
    void stop();

signals:
    void started();
    void stopped();

    void isStartedChanged(bool isStarted);
    void currentTestChanged(AbstractTest* currentTest);

protected:
    class Private;
    friend class Private;
    Private* d;
};

#endif // TESTSCHEDULER_H
