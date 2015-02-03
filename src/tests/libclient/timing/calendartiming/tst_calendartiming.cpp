#include <QtTest>

#include <timing/calendartiming.h>

class TestCalendarTiming : public QObject
{
    Q_OBJECT

private slots:
    void constructors()
    {
        CalendarTiming timing;

        QCOMPARE(timing.type(), QLatin1String("calendar"));
    }

    void nextRuns()
    {
        QTime time = QTime::currentTime();
        time = QTime(time.hour(), time.minute(), time.second());
        QDateTime now = QDateTime::currentDateTime();
        now.setTime(time);

        QDateTime start = QDateTime::currentDateTime();
        QDateTime end;
        QList<int> months = QList<int>()<<1<<2<<3<<4<<5<<6<<7<<8<<9<<10<<11<<12;
        QList<int> daysOfWeek = QList<int>()<<1<<2<<3<<4<<5<<6<<7;
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

        if (now.time().hour() > 3)
        {
            // start tomorrow because hour already passed today (not true before 4 am)
            CalendarTiming tomorrowTiming(start, end, months, daysOfWeek, daysOfMonth, QList<int>()<<4, minutes, seconds);
            qDebug("nextRun tomorrow because hour already passed today (4 am)");
            QDateTime tomorrowTime = now.addDays(1);
            tomorrowTime.setTime(QTime(4,0,0));
            QCOMPARE(tomorrowTiming.nextRun(), tomorrowTime);

            // start tomorrow because hour already passed today (not true before 4 am [2])
            CalendarTiming tomorrowTiming2(start, end, months, daysOfWeek, daysOfMonth, QList<int>()<<4<<5, minutes, seconds);
            qDebug("nextRun tomorrow because hour already passed today (4 and 5 am)");
            QCOMPARE(tomorrowTiming2.nextRun(), tomorrowTime);
        }

        if (now.date().day() > 1)
        {
            // start next month because day already passed this month (not true on 1st of the month)
            CalendarTiming nextMonthTiming(start, end, months, daysOfWeek, QList<int>()<<1, hours, minutes, seconds);
            qDebug("nextRun next month because day already passed this month (1st)");
            QDateTime nextMonthTime = now.addMonths(1);
            nextMonthTime.setTime(QTime(0,0,0));
            nextMonthTime.setDate(QDate(nextMonthTime.date().year(), nextMonthTime.date().month(), 1));
            QCOMPARE(nextMonthTiming.nextRun(), nextMonthTime);
        }

        // start next month because day already passed this month (not true on 1st or 2nd of the month)
        if (now.date().day() > 2)
        {
            QDateTime nextMonthTime = now.addMonths(1);
            nextMonthTime.setTime(QTime(0,0,0));
            nextMonthTime.setDate(QDate(nextMonthTime.date().year(), nextMonthTime.date().month(), 1));
            CalendarTiming nextMonthTiming2(start, end, months, daysOfWeek, QList<int>()<<1<<2, hours, minutes, seconds);
            qDebug("nextRun next month because day already passed this month (1st and 2nd)");
            QCOMPARE(nextMonthTiming2.nextRun(), nextMonthTime);
        }

        // no nextRun because of invalid Timing (February 30th)
        CalendarTiming februaryTiming(start, end,  QList<int>()<<2, daysOfWeek, QList<int>()<<30, hours, minutes, seconds);
        qDebug("no nextRun because invalid Timing (30.02)");
        QCOMPARE(februaryTiming.nextRun(), QDateTime());

        if (now.date().month() > 1 || now.date().day() > 1)
        {
            // start next year because day already passed this year (not true on January 1st)
            CalendarTiming yearTiming(start, end,  QList<int>()<<1, daysOfWeek, QList<int>()<<1, hours, minutes, seconds);
            qDebug("nextRun on January 1st");
            QCOMPARE(yearTiming.nextRun(), QDateTime(QDate(now.date().year()+1, 1, 1), QTime(0,0,0)));
        }
    }
};

QTEST_MAIN(TestCalendarTiming)

#include "tst_calendartiming.moc"
