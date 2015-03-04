#include "timing.h"

qint64 Timing::timeLeft(const QDateTime &when) const
{
    return when.msecsTo(nextRun());
}

QDateTime Timing::lastExecution()
{
    return m_lastExecution;
}
