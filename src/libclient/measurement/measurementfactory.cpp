#include "measurementfactory.h"

// TODO: Pluginize measurements
#include "btc/btc_ma.h"
#include "btc/btc_mp.h"

MeasurementPtr MeasurementFactory::createMeasurement(const QString &name)
{
    // TODO: Improve creation handling
    if (name == "btc_ma")
        return MeasurementPtr(new BulkTransportCapacityMA);

    if (name == "btc_mp")
        return MeasurementPtr(new BulkTransportCapacityMP);

    return MeasurementPtr();
}
