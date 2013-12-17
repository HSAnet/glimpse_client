#ifndef PACKETTRAINSDEFINITION_H
#define PACKETTRAINSDEFINITION_H

#include <QVariant>
#include <QSharedPointer>

#include "../measurementdefinition.h"

class PacketTrainsDefinition;

typedef QSharedPointer<PacketTrainsDefinition> PacketTrainsDefinitionPtr;
typedef QList<PacketTrainsDefinitionPtr> PacketTrainsDefinitionList;

class PacketTrainsDefinition : public MeasurementDefinition
{
public:
    PacketTrainsDefinition(QString host, quint16 port, quint16 packetSize, quint16 trainLength, quint8 iterations);

    QString host;
    quint16 port;
    quint16 packetSize;
    quint16 trainLength;
    quint8 iterations;

    QVariant toVariant() const;
    static PacketTrainsDefinitionPtr fromVariant(const QVariant &variant);
};

enum msgtype {
        MSG_CTRL,
        MSG_MSRMNT
};

/*
 * to identify out-of-order packets a recv counter could be added
 */
struct msrmnt {
        quint16 iter;  // train number
        quint8 id;  // packet number (sender)
        quint8 r_id;  // packet number (receiver)
        struct timespec otime;  // originate timestamp
        struct timespec rtime;  // received timestamp
} __attribute__((packed));

struct ctrl {
        quint16 iter;
        quint16 size;
} __attribute__((packed));

struct msg {
        enum msgtype type;
        union {
                struct msrmnt m;
                struct ctrl c;
        } data;
} __attribute__((packed));

#endif // PACKETTRAINSDEFINITION_H
