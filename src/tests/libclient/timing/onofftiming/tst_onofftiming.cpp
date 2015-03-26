#include <QtTest>

#include <timing/onofftiming.h>_>

class TestOnOffTiming : public QObject
{
    Q_OBJECT

private slots:
    void nextRuns(){

        QDateTime start = QDateTime::currentDateTime();
        OnOffTiming timing(start);

        QCOMPARE(timing.type(), QLatin1String("onoff"));
        qDebug("start timing now");
        QCOMPARE(timing.nextRun(), start);
        qDebug("reset timing");
        QCOMPARE(timing.reset(), false);
    }
};

QTEST_MAIN(TestOnOffTiming)

#include "tst_onofftiming.moc"
