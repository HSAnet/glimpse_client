#ifndef TEST_H
#define TEST_H

#include "testinfo.h"

#include <QtPlugin>
#include <QByteArray>
#include <QStringList>
#include <QHostAddress>
#include <QUdpSocket>

class AbstractTest : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name CONSTANT)
    Q_PROPERTY(bool isMaster READ isMaster CONSTANT)
    Q_PROPERTY(bool finished READ isFinished NOTIFY finishedChanged)

public:
    explicit AbstractTest(QObject* parent = 0)
    : QObject(parent)
    {
        connect(this, SIGNAL(started()), this, SIGNAL(finishedChanged()));
        connect(this, SIGNAL(stopped()), this, SIGNAL(finishedChanged()));
    }

    virtual QString name() const = 0;
    virtual bool isMaster() const = 0;

    virtual bool initialize(const PeerList& peers, bool master, QUdpSocket* socket) = 0;
    virtual void uninitialize() = 0;

    virtual bool isFinished() const = 0;

    virtual void processDatagram(const QByteArray& datagram, const QHostAddress& host, quint16 port) = 0;

    virtual QVariant result() const = 0;

public slots:
    virtual bool start() = 0;
    virtual bool stop() = 0;

    virtual QVariant data(int role) const = 0;

signals:
    void started();
    void stopped();
    void finished(); // called before stopped()!

    void finishedChanged();
};

Q_DECLARE_INTERFACE(AbstractTest, "de.hs-augsburg.mplane.AbstractTest")

#endif // TEST_H
