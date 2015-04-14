#include "snmp.h"

Snmp::Snmp(QObject *parent) :
    Measurement(parent),
    m_status(Unknown)
{

}

Snmp::~Snmp()
{

}

// get status of measurement
Measurement::Status Snmp::status() const
{
    return m_status;
}

// prepare measurement of snmp probe
bool Snmp::prepare(NetworkManager *networkManager, const MeasurementDefinitionPtr &measurementDefinition)
{
    Q_UNUSED(networkManager);

    return true;
}

// start snmp measurement
bool Snmp::start()
{
    return true;
}

// call to interrupt a running measurement.
// is called when measurement has finished.
bool Snmp::stop()
{
    return true;
}

// return the measurements result.
Result Snmp::result() const
{
    return Result();
}

