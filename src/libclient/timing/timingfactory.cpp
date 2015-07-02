#include "timingfactory.h"
#include "periodictiming.h"
#include "onofftiming.h"
#include "calendartiming.h"
#include "immediatetiming.h"
#include "ondemandtiming.h"
#include "log/logger.h"
#include "client.h"
#include "controller/ntpcontroller.h"

#include <QRegularExpression>

LOGGER(TimingFactory);


TimingPtr TimingFactory::timingFromVariant(const QVariant &variant)
{
    if (!variant.isValid())
    {
        return TimingPtr();
    }

    typedef TimingPtr(*CreateFunction)(const QVariant &);
    QHash<QString, CreateFunction> factories;
    factories.insert("periodic", PeriodicTiming::fromVariant);
    factories.insert("onoff", OnOffTiming::fromVariant);
    factories.insert("calendar", CalendarTiming::fromVariant);
    factories.insert("immediate", ImmediateTiming::fromVariant);
    factories.insert("ondemand", OnDemandTiming::fromVariant);

    QVariantMap hash = variant.toMap();

    if (hash.isEmpty())
    {
        return TimingPtr();
    }

    QHashIterator<QString, CreateFunction> iter(factories);
    while (iter.hasNext())
    {
        iter.next();

        QString type = iter.key();
        QVariant value = hash.value(type);
        if (!value.isNull())
        {
            CreateFunction cf = factories.value(type);
            if (cf)
            {
                return cf(value);
            }
            else
            {
                return TimingPtr();
            }
        }
    }

    return TimingPtr();
}

int seclabel[] = {86400, 3600, 60, 1};

// returns seconds from duration-string
int TimingFactory::parseDuration(QString str)
{
    if (str.isEmpty())
    {
        return -1;
    }

    QRegularExpression re("((\\d+)d)?((\\d+)h)?((\\d+)m)?((\\d+)s)?");

    QRegularExpressionMatch m = re.match(str);
    if (m.isValid())
    {
        int valsec = 0;
        for (int i=0; i<4; i++)
        {
            QString group = m.captured(2*i+1);
            if (!group.isNull())
            {
                group.chop(1);
                valsec += seclabel[i] * group.toInt();
            }
        }

        return valsec;
    }
    else
    {
        LOG_ERROR(QString("%1 does notappear to be an mPlane duration").arg(str));
        return -1;
    }

}

TimingPtr TimingFactory::timingFromMPlaneWhen(const QString &when)
{
    if (when == "now")
    {
        return TimingPtr(new ImmediateTiming());
    }

    QStringList valsplit = when.split("repeat ");
    QString valstr;

    if (valsplit.length() > 1)
    {
        valstr = valsplit[1];
    }
    else
    {
        valstr = valsplit[0];
    }

    int period = -1;
    int duration = -1;
    QDateTime start;
    QDateTime end;

    // split period
    valsplit = valstr.split(" / ");
    if (valsplit.length() > 1)
    {
        // periodic
        valstr = valsplit[0];
        period = parseDuration(valsplit[1]);
        if (period < 0)
        {
            LOG_ERROR("Period not supported");
            return TimingPtr();
        }
    }

    // duration
    valsplit = valstr.split(" + ");
    if (valsplit.length() > 1)
    {
        duration = parseDuration(valsplit[1]);
        if (duration < 0)
        {
            LOG_ERROR("Duration not supported");
            return TimingPtr();
        }
    }
    else // try to split range
    {
        valsplit = valsplit[0].split(" ... ");

        if (valsplit.length() > 1)
        {
            end = QDateTime::fromString(valsplit[1].replace('T', ' '));
            if (!end.isValid())
            {
                LOG_ERROR(QString("End-time not recognized: %1").arg(valsplit[0]));
                return TimingPtr();
            }
        }
    }

    if (valsplit[0] == "now")
    {
        // start is now + 1 second because it takes some time to get to the scheduler
        start = Client::instance()->ntpController()->currentDateTime().addSecs(1);
    }
    else
    {
        start = QDateTime::fromString(valsplit[0].replace('T', ' '));
        if (!start.isValid())
        {
            LOG_ERROR(QString("Start-time not recognized: %1").arg(valsplit[0]));
            return TimingPtr();
        }
    }

    if (duration > 0 && !end.isValid())
    {
        end = start.addSecs(duration);
    }

    if (period)
    {
        return TimingPtr(new PeriodicTiming(period*1000, start, end));
    }
    else if (end.isValid())
    {
        LOG_ERROR("GLIMPSE does not support timings with range/duration without a period.");
        return TimingPtr();
    }
    else if (start.isValid())
    {
        return TimingPtr(new OnOffTiming(start));
    }
    else
    {
        LOG_ERROR("Timing not recognized.");
        return TimingPtr();
    }
}
