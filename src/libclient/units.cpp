#include "units.h"
#include "log/logger.h"

#include <qmath.h>

#include <QScreen>
#include <QGuiApplication>

/// This class is very inspired by ubuntu-ui-toolkit.
///
/// See here for details:
/// http://bazaar.launchpad.net/~ubuntu-sdk-team/ubuntu-ui-toolkit/trunk/view/head:/modules/Ubuntu/Components/plugin/ucunits.h
/// http://bazaar.launchpad.net/~ubuntu-sdk-team/ubuntu-ui-toolkit/trunk/view/head:/modules/Ubuntu/Components/plugin/ucunits.cpp

LOGGER(Units)

#define DEFAULT_GRID_UNIT_PX 18

class Units::Private
{
public:
    Private()
    : gridUnit(DEFAULT_GRID_UNIT_PX)
    {
    }

    float gridUnit;
};

Units::Units(QObject *parent)
: QObject(parent)
, d(new Private)
{
#if defined(Q_OS_IOS) || defined(Q_OS_ANDROID)
    QGuiApplication* app = qobject_cast<QGuiApplication*>(qApp);

    QList<QScreen*> screens = app->screens();
    LOG_DEBUG(QString("Found %1 screens for this device").arg(screens.size()));

    if ( !screens.isEmpty() )
    {
        QScreen* screen = screens.at(0);

        QSize size = screen->size();
        LOG_DEBUG(QString("Resolution is %1x%2").arg(size.width()).arg(size.height()));

        // We don't trust size yet we assume portrait mode
        int width = qMin(size.width(), size.height());

        float unit = width / (768.0/DEFAULT_GRID_UNIT_PX);
        LOG_DEBUG(QString("Changing grid unit to %1").arg(unit));
        setGridUnit(unit);
    }

#else
    setGridUnit(10);
#endif
}

Units::~Units()
{
    delete d;
}

float Units::gu(float value) const
{
    const float ratio = d->gridUnit / DEFAULT_GRID_UNIT_PX;
    if (value <= 2.0)
    {
        // for values under 2dp, return only multiples of the value
        return qRound(value * qFloor(ratio));
    }
    else
    {
        return qRound(value * ratio);
    }
}

float Units::dp(float value) const
{
    return qRound(value * d->gridUnit);
}

void Units::setGridUnit(float gridUnit)
{
    if (d->gridUnit != gridUnit)
    {
        d->gridUnit = gridUnit;
        emit gridUnitChanged();
    }
}

float Units::gridUnit() const
{
    return d->gridUnit;
}
