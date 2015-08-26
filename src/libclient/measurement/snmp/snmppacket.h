#ifndef SNMPROTOCOL_H
#define SNMPROTOCOL_H

/*
  --------------------------------------------------------------------------
  Class SnmpPaket
  --------------------------------------------------------------------------

  This class manages SNMP requests. It can do a synchronous request using
  the net-snmp library and it can build datagrams to send via UDP.
  To use this class it is necassary to initialize the Net-SNMP library.
  Call 'init_snmp()' bevor. The class uses some function from the library.
  These functions will not work unless the library is initialized.
  Works with the SNMP versions 1 and 2c.
  It does not support SNMPv3 yet.
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
// Encodes values as described in the Basic Encoding Rules.
// Customiced for the values 'version' and 'community'.
class TripleBER
{
public:
    TripleBER();

private:
    quint8 m_type;
    // length is part of QByteArray.
    QByteArray m_value;

public:
    quint8 type() const;
    void setType(const quint8 type);
    quint8 length() const;
    QString stringValue() const;
    int intValue() const;
    void setValue(const QString &stringValue);
    void setValue(const long stringValue);
    QByteArray getAsByteArray() const;
    quint16 fromByteArray(const QByteArray &datagram, quint16 position);
};

// SNMP paket
class SnmpPacket
{
public:
    SnmpPacket();
    SnmpPacket(const QString error);
    SnmpPacket(const SnmpPacket &rhs);
    ~SnmpPacket();

    enum ImplicitNull { noSuchObject = 128, noSuchInstance, endOfMibView };
    enum Authentication { NoneAuth, MD5 };
    enum Privacy { NonePrivacy, AES, DES };

public:
    // Getter and Setter
    long version() const;
    void setVersion(const long version);
    QString community() const;
    void setCommunity(const QString &community);
    void setCommand(const int command);
    QString username() const;
    void setUsername(const QString &username);
    QString host() const;
    void setHost(const QString &host);
    void setAuthentication(Authentication auth);
    Authentication authentication() const;
    QString contextOID() const;
    void setContextOID(const QString &contextObjID);
    QString errorString() const;
    void setPrivacy(const Privacy privacy);
    Privacy privacy() const;
    // Methods
    QByteArray getDatagram();
    QString pduValueAt(const quint8 index) const;
    bool hasPduValueAt(const quint8 index) const;
    int intValueAt(const quint8 index) const;
    QString stringValueAt(const quint8 index) const;
    quint8 valueTypeAt(const quint8 index) const;
    bool addNullValue(const QString &oidString);
    bool addIntegerValue(const QString &oidString, const int type, const int value);
    bool addIntegerValue(const QString &oidString, const int type, const qint64 value);
    bool addStringValue(const QString &oidString, const int type, const QString value);
    bool addValueWithType(const QString &oidString, const QString &value, const int type);
    bool addObjectIdentifierValue(const QString &oidString, const QString value);
    void setBulkGetRepeats(const int repeats);
    void setBulkGetNonRepeaters(const int nonrepeaters);
    QVariantList valueList() const;
    QStringList oidValueList() const;

    // Send a request to an agent.
    SnmpPacket synchRequest(const QString &password = QString());
    SnmpPacket synchRequestGet(const QString &host, const long version, const QString &community, const QStringList &oidList);
    SnmpPacket synchRequestBulkGet(const QString &host, const long version, const QString &community,
                                   const QStringList &oidList, const int repeaters, const int nonrepeaters);

    // Inline functions
    bool isEmpty() const                    { return m_pdu == NULL; }
    bool hasError() const                   { return m_hasError; }

    // Static functions
    static SnmpPacket snmpGetRequest(const long version, const QString &community);
    static SnmpPacket snmpGetRequest(const long version, const QString &community, const QString &objectId);
    static SnmpPacket fromDatagram(const QByteArray &datagram);
    static quint16 lengthValueFromByteArray(const QByteArray &array, quint16 &position);
    static QByteArray lengthValueToByteArray(const int length);
    static SnmpPacket fromPduStruct(snmp_pdu *pduClone);

private:
    long m_version;
    QString m_community;
    QString m_host;
    QByteArray m_username;
    QString m_contextOid;
    struct snmp_pdu *m_pdu;
    bool m_hasError;
    QString m_errorText;
    Authentication m_authentication;
    Privacy m_privacy;

    // Methods
    size_t approximatePduSize();
    variable_list* variableAtIndex(const quint8 index) const;
    snmp_session* getSnmpSession(const QString &password) const;
    QVariant variantValueOf(variable_list *variable) const;
    QString oidValueString(const variable_list *variable) const;
    bool getObjectIdentifier(const QString &oidString, oid *pObjectId, size_t *oidLength) const;
};

#endif // SNMPROTOCOL_H
