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
};

QTEST_MAIN(TestCalendarTiming)

#include "tst_calendartiming.moc"
