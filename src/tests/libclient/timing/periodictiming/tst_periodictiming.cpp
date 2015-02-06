#include <QtTest>

#include <timing/periodictiming.h>

class TestPeriodicTiming : public QObject
{
    Q_OBJECT

private slots:
    void constructors()
    {
        PeriodicTiming timing(10);

        QCOMPARE(timing.type(), QLatin1String("periodic"));
    }

    void nextRuns()
    {
        QDateTime now = QDateTime::currentDateTime();

        // milliseconds, minutes and hours with infinite timing
        PeriodicTiming tenMs(10, QDateTime(), QDateTime(), 0);
        qDebug("nextRun time in 10 milliseconds");
        QCOMPARE(tenMs.nextRun(now), now.addMSecs(10));

        PeriodicTiming tenMinutes(60*1000*10, QDateTime(), QDateTime(), 0);
        qDebug("nextRun in 10 minutes");
        QCOMPARE(tenMinutes.nextRun(now), now.addSecs(60*10));

        PeriodicTiming hundretHours(60*1000*60*100, QDateTime(), QDateTime(), 0);
        qDebug("nextRun in 100 hours");
        QCOMPARE(hundretHours.nextRun(now), now.addSecs(60*60*100));

        // within start and end date
        PeriodicTiming tenMinutesWithinRange(60*1000*10, now.addSecs(-60*10), now.addSecs(60*20), 0);
        qDebug("nextRun in 10 minutes (with start/end)");
        QCOMPARE(tenMinutesWithinRange.nextRun(now), now.addSecs(60*10));

        // start date not range
        PeriodicTiming tenMinutesNotYet(60*1000*10, now.addSecs(60*20), now.addSecs(60*40), 0);
        qDebug("starttime not reached, start in 20 minutes");
        QCOMPARE(tenMinutesNotYet.nextRun(now), now.addSecs(60*20));

        // end date already reached
        PeriodicTiming tenMinutesOutOfRange(60*1000*10, now.addSecs(-60*10), now.addSecs(-10), 0);
        qDebug("endtime already reached, do not start");
        QCOMPARE(tenMinutesOutOfRange.nextRun(now), QDateTime());

        // test of randomness
        PeriodicTiming tenMinutesRand(60*1000*10, QDateTime(), QDateTime(), 10*1000);
        qDebug("nextRun in 10 minutes (+10 seconds randomness)");
        QDateTime nextRun = tenMinutesRand.nextRun(now);
        if (nextRun < now.addSecs(60*10) || nextRun > now.addSecs(60*10+10))
        {
            QFAIL("Expected nextRun to be in 10 minutens +10 seconds randomness, but it was not");
        }
    }
};

QTEST_MAIN(TestPeriodicTiming)

#include "tst_periodictiming.moc"
