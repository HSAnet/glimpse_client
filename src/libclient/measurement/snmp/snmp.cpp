#include "snmp.h"

Snmp::Snmp(QObject *parent) :
    Measurement(parent),
    m_status(Unknown)
{

}

Snmp::~Snmp()
{

}

// SLOT
// Is called when scan result is ready.
void Snmp::finaliseMeasurement()
{
    m_status = Finished;
    result();
}

// SLOT
// Received an error report from scanner.
void Snmp::errorReport(const QString errorMsg)
{
    emit error(errorMsg);
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

    m_definition = measurementDefinition.dynamicCast<SnmpDefinition>();

    if (m_definition.isNull())
    {
        setErrorString(QString("Definition is Null."));
        return false;
    }
    // Check SNMP version number.
    if (m_definition->m_snmpVersion < SNMP_VERSION_1 || m_definition->m_snmpVersion > SNMP_VERSION_3)
    {
        setErrorString(QString("Version number is invalid."));
        return false;
    }
    // Check for SNMP community names if SNMP version is 1 or 2c.
    // Community name is not used in other versions.
    if (m_definition->m_communityList.isEmpty() &&
            (m_definition->m_snmpVersion == SNMP_VERSION_1 || m_definition->m_snmpVersion == SNMP_VERSION_2c))
    {
        setErrorString(QString("No community names defined."));
        return false;
    }
    // Check number of retries per IP address. Is neccassary in measurement type 'ScanRange' or 'Autoscan'.
    if ( (m_definition->m_measurementType == AutoScan || m_definition->m_measurementType == RangeScan) &&
            (m_definition->m_retriesPerIp < 1 || m_definition->m_retriesPerIp > 10) )
    {
        setErrorString(QString("Too much retries defined. Max. number of retries is 10."));
        return false;
    }
    // Check IP address range if measurment scans a range of IP addresses. (measurement type is 'ScanRange').
    if (m_definition->m_measurementType == RangeScan && m_definition->m_startRangeIp > m_definition->m_endRangeIp)
    {
        setErrorString(QString("Start IP address is higher then end IP of range."));
        return false;
    }
    // Connect Signals with Slots
    if (m_definition->m_measurementType == AutoScan || m_definition->m_measurementType == RangeScan)
    {
        QObject::connect(&m_scanner, SIGNAL(scanFinished(const DeviceMap*)), &m_resultCreator, SLOT(createResult(const DeviceMap*)));
        QObject::connect(&m_resultCreator, SIGNAL(scanResultReady()), this, SLOT(finaliseMeasurement()));
        QObject::connect(&m_scanner, SIGNAL(reportError(const QString)), this, SLOT(errorReport(const QString)));
    }

    return true;
}

// start snmp measurement
bool Snmp::start()
{
    m_status = Running;
    switch (m_definition->m_measurementType)
    {
    case AutoScan:
        if (!m_scanner.startScan(m_definition->m_snmpVersion, m_definition->m_communityList, QString("sysDescr.0"),
                                     m_definition->m_retriesPerIp))
        {
            setErrorString(QString("Could not find any online network interface."));
            m_status = Unknown;
            return false;
        }
        break;
    case RangeScan:
        if (m_scanner.scanRange(m_definition->m_snmpVersion, m_definition->m_communityList, QString("sysDescr.0"),
                                m_definition->m_retriesPerIp, QHostAddress(m_definition->m_startRangeIp),
                                QHostAddress(m_definition->m_endRangeIp)))
        {
            setErrorString(QString("Could not create SNMP packet."));
            m_status = Unknown;
            return false;
        }
        break;
    default:
        setErrorString(QString("No measurement of this type available."));
        m_status = Unknown;
        return false;
        break;
    }

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
    switch (m_definition->m_measurementType) {
    case AutoScan:
    case RangeScan:
    {
        QVariantMap resultMap = m_resultCreator.resultMap();
        Result result(resultMap);
        return result;
        break;
    }
    default:
        break;
    }

    return Result();
}

