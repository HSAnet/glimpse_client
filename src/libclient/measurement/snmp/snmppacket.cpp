#include "snmppacket.h"
#include <QDebug>

// Constructs a Simple Network Management Protocol
SnmpPacket::SnmpPacket() :
    m_pdu(NULL)
{

}

// Copy constructor
SnmpPacket::SnmpPacket(const SnmpPacket &rhs) :
    m_version(rhs.m_version),
    m_community(rhs.m_community)
{
    m_pdu = snmp_clone_pdu(rhs.m_pdu);
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
    if (m_version.value().isEmpty())
    {
        return 0;
    }
    long version = (quint8)m_version.value().at(0);
    for (int i=1; i<m_version.value().size(); ++i)
    {
        version = version << 8;
        version += (quint8)m_version.value().at(i);
    }

    return version;
}

// Set Snmp version
void SnmpPacket::setVersion(const long version)
{
    m_version.setType(ASN_INTEGER);
    m_version.setValue(version);
}

QString SnmpPacket::community() const
{
    return QString(m_community.value());
}

void SnmpPacket::setCommunity(const QString &community)
{
    m_community.setType(ASN_OCTET_STR);
    m_community.setValue(community);
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

// Get the SNMP datagram.
QByteArray SnmpPacket::getDatagram()
{
    size_t bufferLength = approximatePduSize();
    size_t outLength = bufferLength;
    u_char *buffer = (u_char*)malloc(bufferLength);
    memset(buffer, 0, bufferLength);
    snmp_pdu_build(m_pdu, buffer, &outLength);
    size_t pduLength = bufferLength - outLength;
    QByteArray version = m_version.getAsByteArray();
    QByteArray community = m_community.getAsByteArray();
    m_messageSequence.setLength(version.size() + community.size() + pduLength);
    QByteArray sequence = m_messageSequence.getAsByteArray();
    QByteArray datagram(sequence);
    datagram.append(version).append(community).append((char*)buffer, pduLength);
    free(buffer);

    return datagram;
}

// Get a value of PDU at index.
// Return a string which reads 'data type: value'.
QString SnmpPacket::pduValueAt(const quint8 index) const
{
    variable_list *variable = variableAtIndex(index);
    if (variable == NULL)
        return QString();
    size_t bufferLen = variable->val_len + 10;
    char *buffer = (char*)malloc(bufferLen);
    memset(buffer, 0, bufferLen);
    snprint_value(buffer, bufferLen, variable->name, variable->name_length, variable);
    QString value(buffer);
    free(buffer);

    return value;
}

// Gets a integer value from PDU at a given index.
// Behaviour is undefined if pdu value is not a int.
int SnmpPacket::intValueAt(const quint8 index) const
{
    variable_list *variable = variableAtIndex(index);
    if (variable == NULL)
        return 0;
    int *value = (int *)variable->buf;

    return *value;
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
    // Get ObjectID of MIB-Object name.
    if (! get_node(oidString.toLocal8Bit().data(), objectId, &oidLength))
    {
        // Get ObjectID of string like ".1.3.6.2.1.1".
        if (! read_objid(oidString.toLocal8Bit().data(), objectId, &oidLength))
        {
            // Error didn't get object ID.
            return false;
        }
    }
    snmp_add_null_var(m_pdu, objectId, oidLength);

    return true;
}

// Get a snmp session object pointer.
// Can be used for a synchronous request.
snmp_session *SnmpPacket::getSnmpSession(const QHostAddress &peerAddress) const
{
    QByteArray community = m_community.value();
    snmp_session session;
    snmp_sess_init(&session);
    session.version = version();
    session.community = (u_char*)community.data();
    session.community_len = community.length();
    QByteArray address = peerAddress.toString().toUtf8();
    session.peername = address.data();

    return snmp_open(&session);
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
    oid objectIdentifier[MAX_OID_LEN];
    size_t identifierLength = MAX_OID_LEN;
    int result = get_node(objectId.toUtf8().data(), objectIdentifier, &identifierLength);
    if (! result)
    {
        result = read_objid(objectId.toUtf8().data(), objectIdentifier, &identifierLength);
        if (! result)
        {
            // Error can not work with given ObjectID.
            return SnmpPacket();
        }
    }
    snmp_add_null_var(packet.m_pdu, objectIdentifier, identifierLength);

    return packet;
}

// Parse a received datagram.
SnmpPacket SnmpPacket::fromDatagram(const QByteArray &datagram)
{
    SnmpPacket paket;
    // Get protocoll head
    quint16 position = paket.m_messageSequence.fromByteArray(datagram, 0);
    if (position == 0)
    {
        // Is not a SNMP datagram it starts not with a sequence.
        return paket;
    }
    position = paket.m_version.fromByteArray(datagram, position);
    position = paket.m_community.fromByteArray(datagram, position);
    // Get Protocoll Data Unit.
    QByteArray pdu = datagram.mid(position);
    size_t pduLength = pdu.size();
    paket.m_pdu = snmp_pdu_create(SNMP_MSG_RESPONSE);
    int result = snmp_pdu_parse(paket.m_pdu, (u_char*)pdu.data(), &pduLength);
    if (result != 0)
    {
        // Could not parse PDU.
        return SnmpPacket();
    }

    return paket;
}

// Get the length value of a byte array. Value is distributed over some bytes.
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

// Get a length value as a byte array. A value higher then 127 must take more then one byte.
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
    pduSize += 20;                  // Add bytes as ensurense

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
quint8 Triple::length() const
{
    return m_value.size();
}

QByteArray Triple::value() const
{
    return m_value;
}

// Set a string value. (community string)
// Set string into a byte array.
void Triple::setValue(const QString &value)
{
    m_value = QByteArray(value.toUtf8());
}

// Set a long value. (snmp version)
// Set long value into a byte array.
void Triple::setValue(const long value)
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
Triple::Triple() :
    m_type(0)
{

}

// Triple
// Copy constructor
Triple::Triple(const Triple &rhs) :
    m_type(rhs.m_type)
{
    m_value.append(rhs.m_value);
}

quint8 Triple::type() const
{
    return m_type;
}

void Triple::setType(const quint8 type)
{
    m_type = type;
}

// Get a TLV triple as byte array. (Type, Length, Value)
// As describted in the Basic Encoding Roles
QByteArray Triple::getAsByteArray() const
{
    QByteArray array;
    array.append(m_type);
    array.append( SnmpPacket::lengthValueToByteArray(m_value.size()) );
    array.append(m_value);

    return array;
}

// Parses the SNMP version or the community string from datagram.
quint16 Triple::fromByteArray(const QByteArray &datagram, quint16 position)
{
    m_type = datagram[position++];
    quint16 length = SnmpPacket::lengthValueFromByteArray(datagram, position);
    m_value = datagram.mid(position, length);

    return position + length;
}
