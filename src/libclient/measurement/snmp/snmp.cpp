#include "snmp.h"
#include "client.h"
#include "connectiontester.h"

Snmp::Snmp(QObject *parent) :
    Measurement(parent),
    m_status(Unknown)
{

}

Snmp::~Snmp()
{
    // Shut down Net-Snmp library.
    snmp_shutdown("glimpse");
}

// SLOT
// Is called when scan result is ready.
void Snmp::snmpMeasurementFinished()
{
    setStatus(Finished);
}

// SLOT
// Received an error report from scanner.
void Snmp::snmpErrorReport(const QString errorMsg)
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
        setErrorString(QString("Too much or too less retries defined. A valid number of retries is 1 to 10."));
        return false;
    }
    // Check IP address range if measurment scans a range of IP addresses. (measurement type is 'ScanRange').
    if (m_definition->m_measurementType == RangeScan)
    {
        if (m_definition->m_startRangeIp.protocol() == QAbstractSocket::IPv4Protocol)
            if (m_definition->m_startRangeIp.toIPv4Address() >= m_definition->m_endRangeIp.toIPv4Address()) {
                setErrorString(QString("Start IP address is higher then end IP of range."));
                return false;
            } else {
                    setErrorString(QString("IPv6 is not supported."));
                    return false;
            }
    }
    // Connect Signals with Slots
    if (m_definition->m_measurementType == AutoScan || m_definition->m_measurementType == RangeScan)
    {
        QObject::connect(&m_scanner, SIGNAL(scanFinished(const DeviceMap*)), &m_resultCreator, SLOT(createResult(const DeviceMap*)));
        QObject::connect(&m_resultCreator, SIGNAL(scanResultReady()), this, SLOT(snmpMeasurementFinished()));
        QObject::connect(&m_scanner, SIGNAL(reportError(const QString)), this, SLOT(snmpErrorReport(const QString)));
        m_resultCreator.setDefaultGateway(getDefaultGateway());
    }

    // Init Net-Snmp library
    init_snmp("glimpse");

    return true;
}

// start snmp measurement
bool Snmp::start()
{
    setStatus(Running);
    switch (m_definition->m_measurementType)
    {
    case AutoScan:
        if (!m_scanner.startScan(m_definition->m_snmpVersion, m_definition->m_communityList, QString("sysDescr.0"),
                                     m_definition->m_retriesPerIp, m_definition->m_sendInterval, m_definition->m_waitTime))
        {
            setErrorString(QString("Could not find any online network interface."));
            setStatus(Error);
            return false;
        }
        break;
    case RangeScan:
        if (m_scanner.scanRange(m_definition->m_snmpVersion, m_definition->m_communityList, QString("sysDescr.0"),
                                m_definition->m_retriesPerIp, m_definition->m_startRangeIp, m_definition->m_endRangeIp,
                                m_definition->m_sendInterval, m_definition->m_waitTime))
        {
            setErrorString(QString("Could not create SNMP packet."));
            setStatus(Error);
            return false;
        }
        break;
    default:
        setErrorString(QString("No measurement of this type available."));
        setStatus(Error);
        return false;
        break;
    }

    return true;
}

// call to interrupt a running measurement.
// is called when measurement has finished.
bool Snmp::stop()
{
    if (m_scanner.stopScanner())
    {
        setStatus(Unknown);
        return true;
    }

    return false;
}

// return the measurements result.
Result Snmp::result() const
{
    Result result;
    switch (m_definition->m_measurementType) {
    case AutoScan:
    case RangeScan:
    {
        QVariantMap resultMap = m_resultCreator.resultMap();
        result = Result(resultMap);
        break;
    }
    default:
        break;
    }

    return result;
}

// Change the current status.
void Snmp::setStatus(Measurement::Status status)
{
    m_status = status;
}

// Get default gateway from client.
QString Snmp::getDefaultGateway()
{
    Client *client = Client::instance();
    ConnectionTester *tester = client->connectionTester();

    return tester->findDefaultGateway();
}
