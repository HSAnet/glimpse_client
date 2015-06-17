#ifndef SNMPROTOCOL_H
#define SNMPROTOCOL_H

/*
  --------------------------------------------------------------------------
  Class SnmpPaket
  --------------------------------------------------------------------------

  This class is meant to create a UDP datagram to send requests to a
  SNMP agent.
  To use this class it is necassary to initialize the Net-SNMP library.
  Call 'init_snmp()' bevor. The class uses some function from the library.
  These functions will not work unless the library is initialized.

  Works with the SNMP versions 1 and 2c.

  */

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <QByteArray>
#include <QString>
#include <QHostAddress>

// Protocol type 'Sequence'.
// As described in the SMI.
class Sequence
{
    quint16 m_length;

    enum SnmpTypes { TypeSequence = 0x30 };

public:
    quint16 length() const;
    void setLength(const quint16 length);
    QByteArray getAsByteArray() const;
    quint16 fromByteArray(const QByteArray &datagram, quint16 position);
};

// TLV (Type, Length, Value) triple.
// Encode values as described in the Basic Encoding Rules.
// Customiced for the values 'version' and 'community'.
class Triple
{
public:
    Triple();
    Triple(const Triple &rhs);

private:
    quint8 m_type;
    // length is part of QByteArray.
    QByteArray m_value;

public:
    quint8 type() const;
    void setType(const quint8 type);
    quint8 length() const;
    QByteArray value() const;
    void setValue(const QString &value);
    void setValue(const long value);
    QByteArray getAsByteArray() const;
    quint16 fromByteArray(const QByteArray &datagram, quint16 position);
};

// SNMP paket
class SnmpPacket
{
public:
    SnmpPacket();
    SnmpPacket(const SnmpPacket &rhs);
    ~SnmpPacket();

public:
    long version() const;
    void setVersion(const long version);
    QString community() const;
    void setCommunity(const QString &community);
    void setCommand(const int command);
    QByteArray getDatagram();
    QString pduValueAt(const quint8 index) const;
    int intValueAt(const quint8 index) const;
    QString stringValueAt(const quint8 index) const;
    quint8 valueTypeAt(const quint8 index) const;
    bool addNullValue(const QString &oidString);
    snmp_session* getSnmpSession(const QHostAddress &peerAddress) const;

    // Inline functions
    bool isEmpty() const                { return m_pdu == NULL; }
    snmp_pdu* pduClone() const               { return snmp_clone_pdu(m_pdu); }

    // Static functions
    static SnmpPacket snmpGetRequest(const long version, const QString &community);
    static SnmpPacket snmpGetRequest(const long version, const QString &community, const QString &objectId);
    static SnmpPacket fromDatagram(const QByteArray &datagram);
    static quint16 lengthValueFromByteArray(const QByteArray &array, quint16 &position);
    static QByteArray lengthValueToByteArray(const int length);
    static SnmpPacket fromPduStruct(snmp_pdu *pduClone);

private:
    Sequence m_messageSequence;
    Triple m_version;
    Triple m_community;
    struct snmp_pdu *m_pdu;

    // Methods
    size_t approximatePduSize();
    variable_list* variableAtIndex(const quint8 index) const;
};

#endif // SNMPROTOCOL_H
