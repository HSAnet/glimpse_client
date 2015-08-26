#include "snmppacket.h"
#include <QDebug>

// Constructs a Simple Network Management Protocol
SnmpPacket::SnmpPacket() :
    m_pdu(NULL),
    m_hasError(false)
{

}

SnmpPacket::SnmpPacket(const QString error) :
    m_hasError(true),
    m_errorText(error),
    m_pdu(NULL)
{

}

// Copy constructor
SnmpPacket::SnmpPacket(const SnmpPacket &rhs) :
    m_version(rhs.m_version),
    m_community(rhs.m_community),
    m_host(rhs.m_host),
    m_contextOid(rhs.m_contextOid),
    m_username(rhs.m_username),
    m_hasError(rhs.m_hasError),
    m_errorText(rhs.m_errorText)
{
    // Clone PDU structure if is present in rhs.
    if (rhs.isEmpty())
    {
        m_pdu = NULL;
    }
    else
    {
        m_pdu = snmp_clone_pdu(rhs.m_pdu);
    }
}

// Destructor
SnmpPacket::~SnmpPacket()
{
    if (m_pdu != NULL)
    {
        snmp_free_pdu(m_pdu);
    }
}

// Get version from QByteArray as long value.
// SNMP_VERSION_1 is returned if version is not set.
long SnmpPacket::version() const
{
    return m_version;
}

// Set Snmp version
void SnmpPacket::setVersion(const long version)
{
    m_version = version;
}

QString SnmpPacket::community() const
{
    return m_community;
}

void SnmpPacket::setCommunity(const QString &community)
{
    m_community = community;
}



// Initialize net-snmp PDU structure.
void SnmpPacket::setCommand(const int command)
{
    if (m_pdu != NULL)
    {
        snmp_free_pdu(m_pdu);
    }
    m_pdu = snmp_pdu_create(command);
}

QString SnmpPacket::username() const
{
    return QString(m_username);
}

void SnmpPacket::setUsername(const QString &username)
{
    m_username = username.toUtf8();
}

QString SnmpPacket::host() const
{
    return m_host;
}

void SnmpPacket::setHost(const QString &host)
{
    m_host = host;
}

void SnmpPacket::setAuthentication(SnmpPacket::Authentication auth)
{
    m_authentication = auth;
}

SnmpPacket::Authentication SnmpPacket::authentication() const
{
    return m_authentication;
}

QString SnmpPacket::contextOID() const
{
    return m_contextOid;
}

void SnmpPacket::setContextOID(const QString &contextObjID)
{
    m_contextOid = contextObjID;
}

QString SnmpPacket::errorString() const
{
    return m_errorText;
}

void SnmpPacket::setPrivacy(const SnmpPacket::Privacy privacy)
{
    m_privacy = privacy;
}

SnmpPacket::Privacy SnmpPacket::privacy() const
{
    return m_privacy;
}

// Get the SNMP datagram.
QByteArray SnmpPacket::getDatagram()
{
    // Build the PDU as byte array.
    size_t bufferLength = approximatePduSize();
    size_t outLength = bufferLength;
    u_char *buffer = (u_char*)malloc(bufferLength);
    memset(buffer, 0, bufferLength);
    snmp_pdu_build(m_pdu, buffer, &outLength);
    size_t pduLength = bufferLength - outLength;
    // Get version as byte array in BER.
    TripleBER versionBER;
    versionBER.setType(ASN_INTEGER);
    versionBER.setValue(m_version);
    QByteArray version = versionBER.getAsByteArray();
    // Get the community string in BER.
    TripleBER communityBER;
    communityBER.setType(ASN_OCTET_STR);
    communityBER.setValue(m_community);
    QByteArray community = communityBER.getAsByteArray();
    // Build the datagram.
    Sequence packetSequence;
    packetSequence.setLength(version.size() + community.size() + pduLength);
    QByteArray sequence = packetSequence.getAsByteArray();
    QByteArray datagram(sequence);
    datagram.append(version).append(community).append((char*)buffer, pduLength);
    free(buffer);

    return datagram;
}

// Get a value of PDU at index.
// Return a string which reads 'OID = data type: value'.
// Like : IF-MIB::ifIndex.23 = INTEGER: 23
QString SnmpPacket::pduValueAt(const quint8 index) const
{
    variable_list *variable = variableAtIndex(index);
    if (variable == NULL)
    {
        return QString();
    }

    return oidValueString(variable);
}

// Tests if PDU has a value at the given index.
// Returns false if index does not exist or has no value.
bool SnmpPacket::hasPduValueAt(const quint8 index) const
{
    variable_list *variable = variableAtIndex(index);
    if (variable == NULL)
    {
        return false;
    }
    if (variable->type == noSuchInstance || variable->type == noSuchObject || variable->type == endOfMibView)
    {
        return false;
    }

    return true;
}

// Gets a integer value from PDU at a given index.
// Behaviour is undefined if pdu value is not a int.
int SnmpPacket::intValueAt(const quint8 index) const
{
    variable_list *variable = variableAtIndex(index);
    if (variable == NULL)
        return 0;

    return *variable->val.integer;
}

// Get a string value from PDU at a given index.
// Behaviour is undefined if pdu value is not a string.
QString SnmpPacket::stringValueAt(const quint8 index) const
{
    variable_list *variable = variableAtIndex(index);
    if (variable == NULL)
        return QString();

    return QString::fromUtf8((char*)variable->val.string, variable->val_len);
}

// Get the data type of a value at a given position.
// Return the ASN.1 number of data type or 0 if an error occures.
quint8 SnmpPacket::valueTypeAt(const quint8 index) const
{
    variable_list *variable = variableAtIndex(index);
    if (variable == NULL)
        return 0;

    return variable->type;
}

// Add a null value to a snmp request.
// Adds a OID (ObjectId) to the PDU with a NULL value.
bool SnmpPacket::addNullValue(const QString &oidString)
{
    size_t oidLength = MAX_OID_LEN;
    oid objectId[oidLength];
    if (!getObjectIdentifier(oidString, objectId, &oidLength)) {
        return false;
    }
    snmp_add_null_var(m_pdu, objectId, oidLength);

    return true;
}

// Set a value of type INTEGER, COUNTER, GAUGE, BOOL, TIMETICKS ... to the PDU.
bool SnmpPacket::addIntegerValue(const QString &oidString, const int type, const int value)
{
    size_t oidLength = MAX_OID_LEN;
    oid objectId[oidLength];
    if (!getObjectIdentifier(oidString, objectId, &oidLength))
    {
        return false;
    }
    snmp_pdu_add_variable(m_pdu, objectId, oidLength, type, &value, sizeof(int));

    return true;
}

// Set a value of type INTEGER64, COUNTER64, ... to the PDU.
bool SnmpPacket::addIntegerValue(const QString &oidString, const int type, const qint64 value)
{
    size_t oidLength = MAX_OID_LEN;
    oid objectId[oidLength];
    if (!getObjectIdentifier(oidString, objectId, &oidLength))
    {
        return false;
    }
    snmp_pdu_add_variable(m_pdu, objectId, oidLength, type, &value, sizeof(qint64));

    return true;
}

// Set a value of type OCTED_STRING, BIT_STRING to the PDU.
bool SnmpPacket::addStringValue(const QString &oidString, const int type, const QString value)
{
    size_t oidLength = MAX_OID_LEN;
    oid objectId[oidLength];
    if (!getObjectIdentifier(oidString, objectId, &oidLength))
    {
        return false;
    }
    QByteArray val = value.toUtf8();
    snmp_pdu_add_variable(m_pdu, objectId, oidLength, type, val.data(), val.size());

    return true;
}

// Add a value to the PDU for a SetRequest.
// Takes a string value and a SNMP value type definition.
// It converts the string the appropriate value type
// and sets these value to the PDU.
bool SnmpPacket::addValueWithType(const QString &oidString, const QString &value, const int type)
{
    switch (type)
    {
    case ASN_UNSIGNED64:
    case ASN_COUNTER64:
    case ASN_INTEGER64: {
        qlonglong val = value.toLongLong();
        return addIntegerValue(oidString, type, val);
        break;
    }
    case ASN_TIMETICKS:
    case ASN_GAUGE:
    case ASN_COUNTER:
    case ASN_INTEGER:
    case ASN_BOOLEAN: {
        int val = value.toInt();
        return addIntegerValue(oidString, type, val);
        break;
    }
    case ASN_BIT_STR:
    case ASN_OCTET_STR:
        return addStringValue(oidString, type, value);
        break;
    case ASN_OBJECT_ID:
        return addObjectIdentifierValue(oidString, value);
        break;
    case ASN_IPADDRESS: {
        int val = QHostAddress(value).toIPv4Address();
        return addIntegerValue(oidString, type, val);
        break;
    }
    default:
        return false;
    }

    return true;
}

// Set an Object Identifier value to the PDU.
bool SnmpPacket::addObjectIdentifierValue(const QString &oidString, const QString value)
{
    size_t oidLength = MAX_OID_LEN;
    oid objectId[oidLength];
    if (!getObjectIdentifier(oidString, objectId, &oidLength))
    {
        return false;
    }
    size_t valOidLength = MAX_OID_LEN;
    oid valObjectId[valOidLength];
    if (!getObjectIdentifier(value, valObjectId, &valOidLength))
    {
        return false;
    }
    snmp_pdu_add_variable(m_pdu, objectId, oidLength, ASN_OBJECT_ID, valObjectId, valOidLength);

    return true;
}

// Get bulk is meant to read table content from MIB.
// Repeats are the number rows to read from MIB.
// PDU command must be SNMP_MSG_GETBULK.
// The PDU field errindex is used to define this value.
void SnmpPacket::setBulkGetRepeats(const int repeats)
{
    if (m_pdu == NULL || m_pdu->command != SNMP_MSG_GETBULK)
    {
        return;
    }
    m_pdu->errindex = repeats;
}

// Get bulk is meant to read table content from MIB.
// Non repeaters are single values to read. (Scalar Object)
// PDU command must be SNMP_MSG_GETBULK.
// The PDU field errstat is used to define this value.
void SnmpPacket::setBulkGetNonRepeaters(const int nonrepeaters)
{
    if (m_pdu == NULL || m_pdu->command != SNMP_MSG_GETBULK)
    {
        return;
    }
    m_pdu->errstat = nonrepeaters;
}

// Get a list of all PDU var-bind content of a response.
QVariantList SnmpPacket::valueList() const
{
    QVariantList list;
    variable_list *variable = m_pdu->variables;
    while (variable)
    {
        if (variable->type == noSuchObject || variable->type == noSuchInstance
                || variable->type == endOfMibView || variable->type == ASN_NULL)
        {
            // Value in PDU is not available or is a NULL value.
            list << QVariant();
            continue;
        }
        list << variantValueOf(variable);

        variable = variable->next_variable;
    }

    return list;
}

// Return the whole list of values in PDU.
// An entry of list consists of OID and value.
// Format:  'IF-MIB::ifIndex.12 = INTEGER: 12'
QStringList SnmpPacket::oidValueList() const
{
    QStringList list;
    variable_list *variable = m_pdu->variables;
    while (variable) {
        list << oidValueString(variable);
        variable = variable->next_variable;
    }

    return list;
}

// Do a request with current settings.
// Return a SnmpPacket with the response or an error message.
SnmpPacket SnmpPacket::synchRequest(const QString &password)
{
    if (m_pdu == NULL)
    {
        return SnmpPacket(QString("Packet has no PDU."));
    }
    snmp_session *session = getSnmpSession(password);
    if (session == NULL)
    {
        return SnmpPacket(QString("Could not create a session."));
    }
    snmp_pdu *response = 0;
    snmp_pdu *request = snmp_clone_pdu(m_pdu);
    int result = snmp_synch_response(session, request, &response);
    snmp_close(session);
    if (result != STAT_SUCCESS)
    {
        return SnmpPacket(QString("Didn't get a response."));
    }

    return SnmpPacket::fromPduStruct(response);
}

// Do a SNMP synchronous request with given parameter and a list of OID's.
// Return a SnmpPacket with the response or an instance with an error message.
SnmpPacket SnmpPacket::synchRequestGet(const QString &host, const long version, const QString &community, const QStringList &oidList)
{
    m_host = host;
    m_version = version;
    m_community = community;
    setCommand(SNMP_MSG_GET);
    // Set requested OID's to PDU.
    foreach (QString objectId, oidList) {
        if (! addNullValue(objectId))
        {
            return SnmpPacket(QString("Could not add these OID: ").append(objectId));
        }
    }

    return synchRequest();
}

// Do a SNMP synchronous bulk get request with given parameter.
// Return a SnmpPacket with the agents response or an instance with an error message.
SnmpPacket SnmpPacket::synchRequestBulkGet(const QString &host, const long version, const QString &community,
                                           const QStringList &oidList, const int repeaters, const int nonrepeaters)
{
    m_host = host;
    m_version = version;
    m_community = community;
    setCommand(SNMP_MSG_GETBULK);
    setBulkGetNonRepeaters(nonrepeaters);
    setBulkGetRepeats(repeaters);
    // Set requested OID's to PDU.
    foreach (QString objectId, oidList) {
        if (! addNullValue(objectId))
        {
            return SnmpPacket(QString("Could not add these OID: ").append(objectId));
        }
    }

    return synchRequest();
}

// Get a snmp session object pointer.
// Can be used for a synchronous request.
snmp_session *SnmpPacket::getSnmpSession(const QString &password) const
{
    QByteArray community = m_community.toUtf8();
    QByteArray host = m_host.toUtf8();
    QByteArray passwd = password.toUtf8();
    snmp_session session;
    snmp_sess_init(&session);
    session.version = m_version;
    session.community = (u_char*)community.data();
    session.community_len = community.length();
    session.peername = host.data();
    if (m_version == SNMP_VERSION_3)
    {
#ifdef NETSNMP_USE_OPENSSL
        if (m_authentication == NoneAuth && m_privacy == NonePrivacy)
            session.securityLevel = SNMP_SEC_LEVEL_NOAUTH;
        if (m_authentication != NoneAuth && m_privacy == NonePrivacy)
            session.securityLevel = SNMP_SEC_LEVEL_AUTHNOPRIV;
        if (m_authentication != NoneAuth && m_privacy != NonePrivacy)
            session.securityLevel SNMP_SEC_LEVEL_AUTHPRIV;
        if (m_authentication = MD5)
        {
            session.securityAuthKey = usmHMACMD5AuthProtocol;
            session.securityAuthProtoLen = sizeof(usmHMACMD5AuthProtocol)/sizeof(oid);
            session.securityAuthKeyLen = USM_AUTH_KU_LEN;
            generate_Ku(session.securityAuthProto,
                               session.securityAuthProtoLen,
                               (u_char *) passwd.data(), passwd.length(),
                               session.securityAuthKey,
                               &session.securityAuthKeyLen)
        }
        // Privacy should be implemented hier.
#endif
    }

    return snmp_open(&session);
}

// Get a QVariant value from PDU variable.
// Not all typse are implemented. If a value type is not
// implemented the function returns an empty QVariant.
QVariant SnmpPacket::variantValueOf(variable_list *variable) const
{
    switch (variable->type) {
    case ASN_BIT_STR:
        return QVariant(QString::fromUtf8((char*)variable->val.bitstring, variable->val_len));
        break;
    case ASN_OCTET_STR:
        return QVariant(QString::fromUtf8((char*)variable->val.string, variable->val_len));
        break;
    case ASN_GAUGE:
    case ASN_INTEGER:
    case ASN_COUNTER:
        return QVariant((int)*variable->val.integer);
        break;
    case ASN_COUNTER64:
    case ASN_INTEGER64:
    {
        quint64 counterVal = variable->val.counter64->high;
        counterVal = counterVal << 32;
        counterVal += variable->val.counter64->low;
        return QVariant(counterVal);
        break;
    }
    case ASN_OBJECT_ID:
    {
        QString objectId;
        for (int i=0; i<variable->val_len; ++i)
        {
            QString position = QString(".%1").arg(variable->val.objid[i]);
            objectId.append(position);
        }
        return QVariant(objectId);
        break;
    }
    case ASN_FLOAT:
        return QVariant((float)*variable->val.floatVal);
        break;
    case ASN_DOUBLE:
        return QVariant((double)*variable->val.floatVal);
        break;
    case ASN_BOOLEAN:
        return QVariant((*variable->val.integer == 1) ? true : false );
    default:
        break;
    }

    return QVariant();
}

// Return a string containing OID and Value.
// Like :   IF-MIB::ifIndex.1 = INTEGER: 1
QString SnmpPacket::oidValueString(const variable_list *variable) const
{
    size_t bufferLen = variable->val_len + 20;
    char buffer[bufferLen];
    int length = snprint_value(buffer, bufferLen, variable->name, variable->name_length, variable);

    return QString::fromUtf8(buffer, length);
}

// Get the OID out of a string. The oid array of pObjectId must have size of oidLength.
bool SnmpPacket::getObjectIdentifier(const QString &oidString, oid *pObjectId, size_t *oidLength) const
{
    QByteArray array = oidString.toLocal8Bit();
    // Get ObjectID of MIB-Object name.
    if (! get_node(array.data(), pObjectId, oidLength))
    {
        // Get ObjectID of string like ".1.3.6.2.1.1".
        if (! read_objid(array.data(), pObjectId, oidLength))
        {
            // Error didn't get object ID.
            return false;
        }
    }
}

// Factory function. Creates a SNMP packet for a get request without PDU values.
SnmpPacket SnmpPacket::snmpGetRequest(const long version, const QString &community)
{
    SnmpPacket packet;
    packet.setCommand(SNMP_MSG_GET);
    packet.setVersion(version);
    packet.setCommunity(community);

    return packet;
}

// Factory function. Creates a SNMP packet for a get request.
SnmpPacket SnmpPacket::snmpGetRequest(const long version, const QString &community, const QString &objectId)
{
    SnmpPacket packet;
    packet.setCommand(SNMP_MSG_GET);
    packet.setVersion(version);
    packet.setCommunity(community);
    if (! packet.addNullValue(objectId))
    {
        return SnmpPacket(QString("Could not add these OID."));
    }

    return packet;
}

// Parse a received datagram.
SnmpPacket SnmpPacket::fromDatagram(const QByteArray &datagram)
{
    SnmpPacket packet;
    // Get protocoll head
    Sequence sequence;
    quint16 position = sequence.fromByteArray(datagram, 0);
    if (position == 0)
    {
        return SnmpPacket(QString("Datagram is not a SNMP packet."));
    }
    TripleBER version;
    position = version.fromByteArray(datagram, position);
    packet.m_version = version.intValue();
    TripleBER community;
    position = community.fromByteArray(datagram, position);
    packet.m_community = community.stringValue();
    // Get Protocoll Data Unit.
    QByteArray pdu = datagram.mid(position);
    size_t pduLength = pdu.size();
    packet.m_pdu = snmp_pdu_create(SNMP_MSG_RESPONSE);
    int result = snmp_pdu_parse(packet.m_pdu, (u_char*)pdu.data(), &pduLength);
    if (result != 0)
    {
        return SnmpPacket(QString("Could not parse PDU in datagram."));
    }

    return packet;
}

// Get the length value of a byte array. Reads the BER encoded length value.
// Is used of Sequence and TripleBER classes.
quint16 SnmpPacket::lengthValueFromByteArray(const QByteArray &array, quint16 &position)
{
    quint8 higestBit = 128;
    quint8 lengthVal = array[position];
    if (lengthVal < higestBit)
    {
        // Byte at position is length value.
        ++position;
        return lengthVal;
    }
    // Length value needs more than one byte.
    quint16 length = 0;
    quint8 numFields = lengthVal - higestBit;
    ++position;
    for (quint8 index=0; index<numFields; ++index)
    {
        length = (length << 8) + (quint8)array[position++];
    }

    return length;
}

// Get a length value as a byte array. Encode a length value to BER.
// Length beneth 128 is set to one byte. Hihger values are spread over
// more than one byte.
QByteArray SnmpPacket::lengthValueToByteArray(const int length)
{
    QByteArray array;
    if (length < 128)
    {
        // Length is less than 128 bytes. It needs just one byte.
        array.append((char)length);
        return array;
    }
    // Length value is higher or equal than 128 bytes.
    array.append((char)128 + 2);                // Set highest bit and define two fields for length value.
    array.append((char)length / 255);           // Set high byte.
    array.append((char)length % 255);           // Set low byte.

    return array;
}

// Get a SnmpPacket object from a PDU structure pointer.
// PDU pointer is adapted and must NOT be freed.
SnmpPacket SnmpPacket::fromPduStruct(snmp_pdu *pdu)
{
    SnmpPacket packet;
    packet.setVersion(pdu->version);
    if (pdu->version == SNMP_VERSION_1 || pdu->version == SNMP_VERSION_2c)
    {
        packet.setCommunity(QString::fromUtf8((char*)pdu->community, pdu->community_len));
    }
    packet.m_pdu = pdu;

    return packet;
}

// Get approximate size of PDU.
size_t SnmpPacket::approximatePduSize()
{
    size_t pduSize = 4 + 3 + 3 + 3; // Fields :    Type + RequestId + Error + ErrIndex
    variable_list *list = m_pdu->variables;
    while (list)
    {
        pduSize += 2 + list->name_length;   // Type and Length field + value
        pduSize += 2 + list->val_len;       // Type and Length field + value
        list = list->next_variable;
    }
    pduSize += 20;                  // Add bytes to ensure that the buffer is not to small.

    return pduSize;
}

// Return a variable_list pointer of a variable at given index or null.
variable_list *SnmpPacket::variableAtIndex(const quint8 index) const
{
    variable_list *list = m_pdu->variables;
    quint8 pos = 0;
    while (pos < index) {
        if (list == NULL)
            break;
        list = list->next_variable;
        ++pos;
    }

    return list;
}


// Getter and setter
quint16 Sequence::length() const
{
    return m_length;
}

void Sequence::setLength(const quint16 length)
{
    m_length = length;
}

// Get the Sequnce as a byte array to build the protocol header.
QByteArray Sequence::getAsByteArray() const
{
    QByteArray array;
    array.append((char)TypeSequence);       // Protocols sequence mark.
    array.append( SnmpPacket::lengthValueToByteArray(m_length) );

    return array;
}

// Get the paket length and return the position of next value in datagram.
quint16 Sequence::fromByteArray(const QByteArray &datagram, quint16 position)
{
    if (datagram[position++] != TypeSequence)
    {
        return 0;
    }
    m_length = SnmpPacket::lengthValueFromByteArray(datagram, position);

    return position;
}

// Getter and setter
quint8 TripleBER::length() const
{
    return m_value.size();
}

// Get a string value from byte array.
QString TripleBER::stringValue() const
{
    return QString(m_value);
}

// Get a integer value from byte array.
int TripleBER::intValue() const
{
    int value = (int)m_value.at(0);
    for (int index=1; index<m_value.length(); ++index)
    {
        value = value * 255 + (int)m_value.at(index);
    }

    return value;
}

// Set a string value. (community string)
// Set string into a byte array.
void TripleBER::setValue(const QString &value)
{
    m_value = value.toUtf8();
}

// Set a long value. (snmp version)
// Set long value into a byte array.
void TripleBER::setValue(const long value)
{
    m_value.clear();
    long val = value;
    while (val >= 1)
    {
        m_value.append((char)val % 255);
        val /= 255;
    }
}

// Triple
// Constructor
TripleBER::TripleBER() :
    m_type(0)
{

}

quint8 TripleBER::type() const
{
    return m_type;
}

void TripleBER::setType(const quint8 type)
{
    m_type = type;
}

// Get a TLV triple as byte array. (Type, Length, Value)
// As describted in the Basic Encoding Roles
QByteArray TripleBER::getAsByteArray() const
{
    QByteArray array;
    array.append(m_type);
    array.append( SnmpPacket::lengthValueToByteArray(m_value.size()) );
    array.append(m_value);

    return array;
}

// Parses the SNMP version or the community string from datagram.
quint16 TripleBER::fromByteArray(const QByteArray &datagram, quint16 position)
{
    m_type = datagram[position++];
    quint16 length = SnmpPacket::lengthValueFromByteArray(datagram, position);
    m_value = datagram.mid(position, length);

    return position + length;
}
