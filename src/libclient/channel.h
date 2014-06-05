#ifndef CHANNEL_H
#define CHANNEL_H

#include "serializable.h"
#include "timing/timing.h"
#include <QDateTime>
#include <QSharedPointer>

class Channel;
typedef QSharedPointer<Channel> ChannelPtr;

class CLIENT_API Channel : public Serializable
{
public:
    Channel();
    Channel(const int &id, const QString &target, const QString &certificate, const TimingPtr &timing);
    ~Channel();

    // Storage
    static ChannelPtr fromVariant(const QVariant &variant);

    // Serializable interface
    QVariant toVariant() const;

    void setTarget(const QString &target);

    QString target() const;
    TimingPtr timing() const;

protected:
    class Private;
    Private *d;
};

#endif // CHANNEL_H
