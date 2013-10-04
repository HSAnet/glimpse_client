#include "calendartiming.h"

class CalendarTiming::Private
{
public:
};

CalendarTiming::CalendarTiming()
: d(new Private)
{
}

CalendarTiming::~CalendarTiming()
{
    delete d;
}

bool CalendarTiming::reset() {
    return true;
}

QDateTime CalendarTiming::nextRun() const {
    return QDateTime(); // never
}

QVariant CalendarTiming::toVariant() const {
    QVariantMap hash;
    // TODO: Implementation
    return hash;
}

TimingPtr CalendarTiming::fromVariant(const QVariant &variant) {
    Q_UNUSED(variant);
    return TimingPtr(new CalendarTiming);
}

QString CalendarTiming::type() const
{
    return "calendar";
}
