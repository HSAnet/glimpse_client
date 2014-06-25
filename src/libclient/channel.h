#ifndef CHANNEL_H
#define CHANNEL_H

#include "serializable.h"
#include "timing/timing.h"
#include <QDateTime>
#include <QSharedPointer>

class Channel;
typedef QList<Channel> ChannelList;


class ChannelData : public QSharedData
{
public:
    ChannelData()
    : id(0)
    {
    }

    ChannelData(const ChannelData &other)
    : QSharedData(other)
    , id(other.id)
    , target(other.target)
    , certificate(other.certificate)
    , timing(other.timing)
    {
    }

    int id;
    QString target;
    QString certificate;
    TimingPtr timing;
};


class CLIENT_API Channel : public Serializable
{
public:
    Channel();
    Channel(const Channel &other);
    Channel(const int &id, const QString &target, const QString &certificate, const TimingPtr &timing);

    bool isNull() const;

    // Storage
    static Channel fromVariant(const QVariant &variant);

    // Serializable interface
    QVariant toVariant() const;

    void setTarget(const QString &target);
    QString target() const;

    void setTiming(const TimingPtr &timing);
    TimingPtr timing() const;

private:
    QSharedDataPointer<ChannelData> d;
};

Q_DECLARE_METATYPE(Channel)
Q_DECLARE_TYPEINFO(Channel, Q_MOVABLE_TYPE);
Q_DECLARE_METATYPE(ChannelList)

#endif // CHANNEL_H
