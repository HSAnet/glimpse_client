#include <QtTest>

#include <timing/onofftiming.h>
#include <controller/ntpcontroller.h>

class TestOnOffTiming : public QObject
{
    Q_OBJECT

private slots:
    void nextRuns()
    {
        NtpController ntp;
        ntp.init();

        QDateTime start = ntp.currentDateTime();
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
