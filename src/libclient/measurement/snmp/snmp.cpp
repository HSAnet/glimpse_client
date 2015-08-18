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
    emit finished();
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
        QObject::connect(&m_scanner, SIGNAL(reportError(const QString)), this, SLOT(snmpErrorReport(const QString)));
        m_resultCreator.setDefaultGateway(getDefaultGateway());
    }

    // Signal result ready.
    QObject::connect(&m_resultCreator, SIGNAL(creatorResultReady()), this, SLOT(snmpMeasurementFinished()));

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
    case GatewayStatistics:
        return getGatewaySatistics();
        break;
    case SingleRequest:
        return userSingleRequest();
        break;
    default:
        setErrorString(QString("No measurement of this type available."));
        setStatus(Error);
        return false;
        break;
    }
    emit started();

    return true;
}

// call to interrupt a running measurement.
// is called when measurement has finished.
bool Snmp::stop()
{
    switch (m_definition->m_measurementType)
    {
    case AutoScan:
    case RangeScan:
        if (m_scanner.stopScanner())
        {
            setStatus(Unknown);
            return true;
        }
        break;
    default:
        break;
    }

    return false;
}

// return the measurements result.
Result Snmp::result() const
{
    QVariantMap resultMap = m_resultCreator.resultMap();

    return Result(resultMap);
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

// Reads statistics of the default gateway if posible.
bool Snmp::getGatewaySatistics()
{
    QHostAddress host(getDefaultGateway());
    QString community("public");
    QStringList oidList = QStringList() << "ifNumber.0" << "ipForwarding.0";
    SnmpPacket request;
    SnmpPacket response = request.synchRequestGet(host, SNMP_VERSION_2c, community, oidList);
    if (response.hasError() || response.isEmpty())
    {
        setErrorString("Can not read Gateway statistics.");
        return false;
    }
    if (response.intValueAt(1) != 1)
    {
        setErrorString("Default gateway addresses not a router.");
        return false;
    }
    int ifNumber = response.intValueAt(0);

    // Read interface index list from interface table.
    response = request.synchRequestBulkGet(host, SNMP_VERSION_2c, community, QStringList() << "ifIndex", ifNumber, 0);
    QVariantList indexList = response.valueList();
    // Read interface operating status list.
    response = request.synchRequestBulkGet(host, SNMP_VERSION_2c, community, QStringList() << "ifOperStatus", ifNumber, 0);
    QVariantList operatingStatusList = response.valueList();
    // Read out going octed statistics.
    response = request.synchRequestBulkGet(host, SNMP_VERSION_2c, community, QStringList() << "ifOutOctets", ifNumber, 0);
    QVariantList outOctetList = response.valueList();
    // read in going octet statistics.
    response = request.synchRequestBulkGet(host, SNMP_VERSION_2c, community, QStringList() << "ifInOctets", ifNumber, 0);
    QVariantList inOctetList = response.valueList();

    m_resultCreator.createResult(indexList, operatingStatusList, outOctetList, inOctetList);

    return true;
}

// User wants to do a request to an agent.
// (Use Glimpse as a tiny Management System)
bool Snmp::userSingleRequest()
{
    SnmpPacket request;
    request.setVersion(m_definition->m_snmpVersion);
    if (! m_definition->m_communityList.isEmpty())
    {
        request.setCommunity(m_definition->m_communityList.at(0));
    }
    request.setHost(m_definition->m_startRangeIp);
    request.setUsername(m_definition->m_username);
    request.setAuthentication((SnmpPacket::Authentication)m_definition->m_authentication);
    request.setPrivacy((SnmpPacket::Privacy)m_definition->m_privacy);
    request.setContextOID(m_definition->m_contextOID);
    SnmpPacket response = request.synchRequest(m_definition->m_password);
    if (response.hasError())
    {
        setErrorString(response.errorString());
        return false;
    }
    if (response.isEmpty())
    {
        setErrorString(QString("Unknown error."));
        return false;
    }

    m_resultCreator.createResult(response, m_definition->m_startRangeIp);

    return true;
}
