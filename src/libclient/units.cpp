#include "units.h"

#include <qmath.h>

/// This class is very inspired by ubuntu-ui-toolkit.
///
/// See here for details:
/// http://bazaar.launchpad.net/~ubuntu-sdk-team/ubuntu-ui-toolkit/trunk/view/head:/modules/Ubuntu/Components/plugin/ucunits.h
/// http://bazaar.launchpad.net/~ubuntu-sdk-team/ubuntu-ui-toolkit/trunk/view/head:/modules/Ubuntu/Components/plugin/ucunits.cpp

#define DEFAULT_GRID_UNIT_PX 8

class Units::Private
{
public:
    Private()
    : gridUnit(DEFAULT_GRID_UNIT_PX)
    {
    }

    int gridUnit;
};

Units::Units(QObject *parent)
: QObject(parent)
, d(new Private)
{
}

Units::~Units()
{
    delete d;
}

float Units::gu(float value) const
{
    const float ratio = d->gridUnit / DEFAULT_GRID_UNIT_PX;
    if (value <= 2.0) {
        // for values under 2dp, return only multiples of the value
        return qRound(value * qFloor(ratio));
    } else {
        return qRound(value * ratio);
    }
}

float Units::dp(float value) const
{
    return qRound(value * d->gridUnit);
}

void Units::setGridUnit(float gridUnit)
{
    if (d->gridUnit != gridUnit) {
        d->gridUnit = gridUnit;
        emit gridUnitChanged();
    }
}

float Units::gridUnit() const
{
    return d->gridUnit;
}
