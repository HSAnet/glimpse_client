#include "snmppaket.h"

// Constructs a Simple Network Management Protocol
SnmpPaket::SnmpPaket()
{

}

// Destructor
SnmpPaket::~SnmpPaket()
{

}

// Get version from QByteArray as long value.
long SnmpPaket::version() const
{
    long version = m_version.value().at(0);
    for (int i=1; i<m_version.value().size(); ++i)
    {
        version = version << 8;
        version += m_version.value().at(i);
    }

    return version;
}

// Set Snmp version
void SnmpPaket::setVersion(const long version, const int length)
{
    m_version.setType(ASN_INTEGER);
    m_version.setValue(version, length);
}

QString SnmpPaket::community() const
{
    return QString(m_community.value());
}

void SnmpPaket::setCommunity(const QString &community)
{
    m_community.setType(ASN_OCTET_STR);
    m_community.setValue(community);
}



// Initialize net-snmp PDU structure.
void SnmpPaket::setCommand(const int command)
{
    memset(&m_pdu, 0, sizeof(snmp_pdu));
    m_pdu.version = SNMP_DEFAULT_VERSION;
    m_pdu.command = command;
    m_pdu.errstat = SNMP_DEFAULT_ERRSTAT;
    m_pdu.errindex = SNMP_DEFAULT_ERRINDEX;
    m_pdu.securityModel = SNMP_DEFAULT_SECMODEL;
    m_pdu.transport_data = NULL;
    m_pdu.transport_data_length = 0;
    m_pdu.securityNameLen = 0;
    m_pdu.contextNameLen = 0;
    m_pdu.time = 0;
    m_pdu.reqid = snmp_get_next_reqid();
    m_pdu.msgid = snmp_get_next_msgid();
}

// Get the SNMP datagram.
QByteArray SnmpPaket::getDatagram()
{
    size_t bufferLength = approximatePduSize();
    size_t outLength = bufferLength;
    u_char *buffer = (u_char*)malloc(bufferLength);
    snmp_pdu_build(&m_pdu, buffer, &outLength);
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
QString SnmpPaket::pduValue(const quint8 index) const
{
    variable_list *list = variableAtindex(index);
    if (list == NULL)
        return QString();
    size_t bufferLen = list->val_len + 10;
    char *buffer = (char*)malloc(bufferLen);
    memset(buffer, 0, bufferLen);
    snprint_value(buffer, bufferLen, list->name, list->name_length, list);
    QString value(buffer);
    free(buffer);

    return value;
}

// Factory function. Creates a SNMP paket for a get request.
SnmpPaket SnmpPaket::snmpGetRequest(const long version, const QString &community, const QString &objectId)
{
    SnmpPaket paket;
    paket.setCommand(SNMP_MSG_GET);
    paket.setVersion(version, sizeof(long));
    paket.setCommunity(community);
    oid objectIdentifier[MAX_OID_LEN];
    size_t identifierLength = MAX_OID_LEN;
    get_node(objectId.toUtf8().data(), objectIdentifier, &identifierLength);
    snmp_add_null_var(&paket.m_pdu, objectIdentifier, identifierLength);

    return paket;
}

// Parse a received datagram.
SnmpPaket SnmpPaket::fromDatagram(const QByteArray &datagram)
{
    SnmpPaket paket;
    // Get protocoll head
    quint16 position = paket.m_messageSequence.fromByteArray(datagram, 0);
    if (position == 0)
    {
        // Is not a SNMP datagram it starts not with a sequence.
        paket.setCommand(SNMP_MSG_RESPONSE);
        return paket;
    }
    position = paket.m_version.fromByteArray(datagram, position);
    position = paket.m_community.fromByteArray(datagram, position);
    // Get Protocoll Data Unit.
    QByteArray pdu = datagram.mid(position);
    size_t pduLength = pdu.size();
    snmp_pdu_parse(&paket.m_pdu, (u_char*)pdu.data(), &pduLength);

    return paket;
}

// Get the length value of a byte array. Value is distributed over some bytes.
quint16 SnmpPaket::lengthValueFromByteArray(const QByteArray &array, quint16 &position)
{
    quint8 higestBit = 128;
    if (array[position] < higestBit)
    {
        return array[position++];
    }
    quint16 length = 0;
    quint8 numFields = array[position++] - higestBit;
    for (quint8 index=0; index<numFields; ++index)
    {
        length = (length << 8) + array[position++];
    }

    return length;
}

// Get a length value as a byte array. A value higher then 127 must take more then one byte.
QByteArray SnmpPaket::lengthValueToByteArray(const int length)
{
    QByteArray array;
    if (length < 128)
    {
        array.append((char)length);
        return array;
    }
    array.append((char)128 + 2);                // Set highest bit and define two fields for length value.
    array.append((char)length / 255);           // Set high byte.
    array.append((char)length % 255);           // Set low byte.

    return array;
}

// Get approximate size of PDU.
size_t SnmpPaket::approximatePduSize()
{
    size_t pduSize = 4 + 3 + 3 + 3; // Sequence + RequestId + Error + ErrIndex
    pduSize += snmp_varbind_len(&m_pdu);
    pduSize += 10;                  // Add 50 bytes as ensurense

    return pduSize;
}

// Return a variable_list pointer of a variable at given index or null.
variable_list *SnmpPaket::variableAtindex(const quint8 index) const
{
    variable_list *list = m_pdu.variables;
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
    array.append( SnmpPaket::lengthValueToByteArray(m_length) );

    return array;
}

// Get the paket length and return the position of next value in datagram.
quint16 Sequence::fromByteArray(const QByteArray &datagram, quint16 position)
{
    if (datagram[position++] != TypeSequence)
    {
        return 0;
    }
    m_length = SnmpPaket::lengthValueFromByteArray(datagram, position);

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
void Triple::setValue(const QString &value)
{
    m_value = QByteArray(value.toUtf8());
}

// Set a long value. (snmp version)
void Triple::setValue(const long value, int length)
{
    int startPosition = sizeof(long) - length;
    char *currentPosition = ((char*)value) + startPosition;
    while (length >= 0)
    {
        m_value.append(*currentPosition);
        ++currentPosition;
        --length;
    }
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
QByteArray Triple::getAsByteArray() const
{
    QByteArray array = SnmpPaket::lengthValueToByteArray(m_value.size());
    array.append(m_type);
    array.append(m_value);

    return array;
}

// Parses the SNMP version or the community string from datagram.
quint16 Triple::fromByteArray(const QByteArray &datagram, quint16 position)
{
    m_type = datagram[position++];
    quint16 length = SnmpPaket::lengthValueFromByteArray(datagram, position);
    m_value = datagram.mid(position, length);

    return position + length;
}
