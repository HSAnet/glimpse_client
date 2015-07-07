#include <QtTest>

#include <timing/calendartiming.h>

#include <controller/ntpcontroller.h>

class TestCalendarTiming : public QObject
{
    Q_OBJECT

private slots:
    void nextRuns()
    {

        NtpController ntp;
        ntp.init();

        QDateTime now = ntp.currentDateTime();

        QTime time = now.time();
        time = QTime(time.hour(), time.minute(), time.second()); // to remove milliseconds
        now.setTime(time);

        QDateTime start = ntp.currentDateTime();
        QDateTime end;
        QList<int> months = CalendarTiming::AllMonths;
        QList<int> daysOfWeek =  CalendarTiming::AllDaysOfWeek;
        QList<int> daysOfMonth;
        QList<int> hours;
        QList<int> minutes;
        QList<int> seconds;

        for (int i = 1; i < 32; i++)
        {
            daysOfMonth<<i;
        }

        for (int i = 0; i < 24; i++)
        {
            hours<<i;
        }

        for (int i = 0; i < 60; i++)
        {
            minutes<<i;
            seconds<<i;
        }
        // start now
        CalendarTiming nowTiming(start, end, months, daysOfWeek, daysOfMonth, hours, minutes, seconds);
        QCOMPARE(nowTiming.type(), QLatin1String("calendar"));
        qDebug("start time now");
        QCOMPARE(nowTiming.nextRun(), now);

        // start time in the future
        CalendarTiming futureTiming(start.addDays(1), end, months, daysOfWeek, daysOfMonth, hours, minutes, seconds);
        qDebug("start time one day in the future");
        QCOMPARE(futureTiming.nextRun(), now.addDays(1));

        // end time already reached
        CalendarTiming pastTiming(start.addDays(1), start.addDays(-1), months, daysOfWeek, daysOfMonth, hours, minutes, seconds);
        qDebug("end time already reached");
        QCOMPARE(pastTiming.nextRun(), QDateTime());

        // start tomorrow because hour already passed today
        CalendarTiming tomorrowTiming(QDateTime(), end, months, daysOfWeek, daysOfMonth, QList<int>()<<4, minutes, seconds);
        qDebug("nextRun tomorrow because hour already passed today (4 am)");
        QDateTime tomorrowTime = now.addDays(1);
        tomorrowTime.setTime(QTime(4,0,0));
        QCOMPARE(tomorrowTiming.nextRun(QDateTime(now.date(), QTime(10,0,0))), tomorrowTime);

        // start tomorrow because hour already passed today
        CalendarTiming tomorrowTiming2(QDateTime(), end, months, daysOfWeek, daysOfMonth, QList<int>()<<4<<5, minutes, seconds);
        qDebug("nextRun tomorrow because hour already passed today (4 and 5 am)");
        QCOMPARE(tomorrowTiming2.nextRun(QDateTime(now.date(), QTime(10,0,0))), tomorrowTime);

        //test if timing at 6am until 2am is wrong
        CalendarTiming sixTiming(start.addDays(-1), end,  months, daysOfWeek, daysOfMonth, QList<int>()<<2<<6, QList<int>()<<0, QList<int>()<<0);
        qDebug("nextRun today at 6");
        QCOMPARE(sixTiming.nextRun(QDateTime(now.date(), QTime(5,0,0))), QDateTime(now.date(), QTime(6,0,0), Qt::UTC));

        //test if timing at 2am until 6am is wrong
        CalendarTiming twoTiming(start.addDays(-1), end,  months, daysOfWeek, daysOfMonth, QList<int>()<<2<<6, QList<int>()<<0, QList<int>()<<0);
        qDebug("nextRun today at 2");
        QCOMPARE(twoTiming.nextRun(QDateTime(now.date(), QTime(1,0,0))), QDateTime(now.date(), QTime(2,0,0), Qt::UTC));

        // start next month because day already passed this month
        CalendarTiming nextMonthTiming(QDateTime(), end, months, daysOfWeek, QList<int>()<<1, hours, minutes, seconds);
        qDebug("nextRun next month because day already passed this month (1st)");
        QDateTime nextMonthTime = now.addMonths(1);
        nextMonthTime.setTime(QTime(0,0,0));
        nextMonthTime.setDate(QDate(nextMonthTime.date().year(), nextMonthTime.date().month(), 1));
        QCOMPARE(nextMonthTiming.nextRun(QDateTime(QDate(now.date().year(), now.date().month(), 2))), nextMonthTime);

        // start next month because day already passed this month
        CalendarTiming nextMonthTiming2(QDateTime(), end, months, daysOfWeek, QList<int>()<<1<<2, hours, minutes, seconds);
        qDebug("nextRun next month because day already passed this month (1st and 2nd)");
        QCOMPARE(nextMonthTiming2.nextRun(QDateTime(QDate(now.date().year(), now.date().month(), 4))), nextMonthTime);

        // no nextRun because of invalid Timing (February 30th)
        CalendarTiming februaryTiming(start, end,  QList<int>()<<2, daysOfWeek, QList<int>()<<30, hours, minutes, seconds);
        qDebug("no nextRun because invalid Timing (30.02)");
        QCOMPARE(februaryTiming.nextRun(), QDateTime());

        // start next year because day already passed this year
        CalendarTiming yearTiming(start, end,  QList<int>()<<1, daysOfWeek, QList<int>()<<1, hours, minutes, seconds);
        qDebug("nextRun on January 1st");
        QCOMPARE(yearTiming.nextRun(QDateTime(QDate(now.date().year(), now.date().month(), 2))), QDateTime(QDate(now.date().year()+1, 1, 1), QTime(0,0,0), Qt::UTC));
    }
};

QTEST_MAIN(TestCalendarTiming)

#include "tst_calendartiming.moc"
