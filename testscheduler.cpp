#include "testscheduler.h"
#include "testfactory.h"

#include <QQueue>
#include <QTimer>

class TestScheduler::Private : public QObject
{
    Q_OBJECT

public:
    Private(TestScheduler* q)
    : q(q)
    , isStarted(false)
    , currentTest(NULL)
    , managerSocket(NULL)
    {
        connect(&timer, SIGNAL(timeout()), this, SLOT(timeout()));

        timer.setInterval(2500);
    }

    TestScheduler* q;

    // Properties
    bool isStarted;
    AbstractTest* currentTest;

    QUdpSocket* managerSocket;
    TestFactory factory;
    QQueue<TestInfo> queue;

    QTimer timer;

    // Functions
    void setStarted(bool started);

public slots:
    void timeout();
};

void TestScheduler::Private::setStarted(bool started)
{
    if ( timer.isActive() == started )
        return;

    emit q->isStartedChanged(started);

    if ( started ) {
        timer.start();
        emit q->started();
    } else {
        timer.stop();
        emit q->stopped();
    }
}

void TestScheduler::Private::timeout()
{
    qDebug() << Q_FUNC_INFO;

    // Don't change the current test if we still run one
    if ( currentTest ) {
        if ( currentTest->isFinished() ) {
            delete currentTest;
            currentTest = NULL;
            emit q->currentTestChanged(NULL);
        } else {
            return;
        }
    }

    if ( queue.isEmpty() ) {
        q->stop();
        return;
    }

    TestInfo info = queue.dequeue();
    AbstractTest* test = factory.createTest(info.name);
    if ( test == NULL ) {
        qDebug() << "There is no test named" << info.name << "- skipping.";
        return;
    }

    if ( !test->initialize(info.peers, info.isMaster, managerSocket) ) {
        qDebug() << "Failed to initialize" << info.name;
        delete test;
        return;
    }

    if ( !test->start() ) {
        qDebug() << "Failed to start" << info.name;
        test->uninitialize();
        delete test;
        return;
    }

    currentTest = test;
    emit q->currentTestChanged(test);
}

TestScheduler::TestScheduler(QObject *parent)
: QObject(parent)
, d(new Private(this))
{
}

TestScheduler::~TestScheduler()
{
    delete d;
}

QUdpSocket *TestScheduler::managerSocket() const
{
    return d->managerSocket;
}

void TestScheduler::setManagerSocket(QUdpSocket *managerSocket)
{
    d->managerSocket = managerSocket;
}

void TestScheduler::enqueue(const TestInfo &info)
{
    qDebug() << Q_FUNC_INFO;

    d->queue.enqueue(info);

    start();
}

bool TestScheduler::isStarted() const
{
    return d->isStarted;
}

AbstractTest *TestScheduler::currentTest() const
{
    return d->currentTest;
}

void TestScheduler::start()
{
    d->setStarted(true);
}

void TestScheduler::stop()
{
    d->setStarted(false);
}

#include "testscheduler.moc"
