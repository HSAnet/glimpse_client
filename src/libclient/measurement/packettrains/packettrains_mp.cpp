#include "packettrains_mp.h"
#include <QUdpSocket>
#include <arpa/inet.h>
#include <iostream>
using namespace std;
#include <iomanip>
#include "../../log/logger.h"
#include "../../network/networkmanager.h"
#include "../../types.h"

LOGGER(PacketTrainsMA);

PacketTrainsMP::PacketTrainsMP()
: m_udpSocket(NULL)
{
}

bool PacketTrainsMP::start()
{
    // Timeout for "nothing happens after start"
    connect(&m_timeout, SIGNAL(timeout()), this, SIGNAL(finished()));
    m_timeout.setInterval(5000);
    m_timeout.setSingleShot(true);
    m_timeout.start();

    // Timer for eval function
    m_timer.setInterval(1000);
    m_timer.setSingleShot(true);
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(eval()));

    // Signal for new packets
    connect(m_udpSocket, SIGNAL(readyRead()), this, SLOT(readPendingDatagrams()));

    return true;
}

void PacketTrainsMP::readPendingDatagrams()
{
    m_timeout.start();

    struct timespec timestamp;
    struct msg* message;
    quint16 iter;

    while(m_udpSocket->hasPendingDatagrams())
    {
        // get time first
#if defined(Q_OS_ANDROID)
        clock_gettime(CLOCK_MONOTONIC_HR, &timestamp);
#else
        clock_gettime(CLOCK_MONOTONIC_RAW, &timestamp);
#endif

        QByteArray buffer;
        buffer.resize(m_udpSocket->pendingDatagramSize());

        QHostAddress sender;
        quint16 senderPort;

        m_udpSocket->readDatagram(buffer.data(), buffer.size(), &sender, &senderPort);
        message = reinterpret_cast<msg*>(buffer.data());

        message->rtime = timestamp;
        message->r_id = m_packetsReceived++;

        iter = ntohs(message->iter);

        if(iter < m_rec.size())
        {
            m_rec[iter].append(*message);
        }
        else
        {
            qDebug() << "error, something went wrong"; // TODO
        }

        m_timer.start();
    }
}

void PacketTrainsMP::eval()
{
    foreach(const QList<msg> &l, m_rec)
    {
        quint64 ts_otime[2] = {0}, ts_rtime[2] = {2};
        double srate = 0, rrate = 0;

        for (int i = 0; i < l.size(); i++) {
            if (l[i].id != l[i].r_id) {
                    qDebug() <<"packages out of order";
            }
        }

        ts_otime[0] = l.first().otime.tv_sec * 1000000000 + l.first().otime.tv_nsec;
        ts_otime[1] = l.last().otime.tv_sec * 1000000000 + l.last().otime.tv_nsec;
        ts_rtime[0] = l.first().rtime.tv_sec * 1000000000 + l.first().rtime.tv_nsec;
        ts_rtime[1] = l.last().rtime.tv_sec * 1000000000 + l.last().rtime.tv_nsec;
        srate = (double) definition->packetSize * l.size() / (ts_otime[1] - ts_otime[0]) * 1000000000;
        rrate = (double) definition->packetSize * l.size() / (ts_rtime[1] - ts_rtime[0]) * 1000000000;
        // ignore infinite rates
        if (ts_otime[0] != ts_rtime[1] && ts_rtime[0] != ts_rtime[1]) {
            m_recvSpeed.append(rrate / 1024);
            m_sendSpeed.append(srate / 1024);
            //cout<<"sending @ "<<fixed<<setprecision(2)<<srate / 1048576 * 8<<" MBit/s"<<endl;
            //cout<<"receiving @ "<<fixed<<setprecision(2)<<rrate / 1048576 * 8<<" MBit/s"<<endl;
        } else {
            LOG_ERROR("Ignoring train due to infinite rate");
        }
    }

    emit finished();
}

void PacketTrainsMP::handleError(QAbstractSocket::SocketError socketError)
{
    if(socketError == QAbstractSocket::RemoteHostClosedError)
        return;

    QAbstractSocket* socket = qobject_cast<QAbstractSocket*>(sender());
    cout<<"Socket Error: "<<socket->errorString().toStdString()<<endl;
}


Measurement::Status PacketTrainsMP::status() const
{
    return Unknown;
}

bool PacketTrainsMP::prepare(NetworkManager *networkManager, const MeasurementDefinitionPtr &measurementDefinition)
{
    Q_UNUSED(networkManager);

    definition = measurementDefinition.dynamicCast<PacketTrainsDefinition>();
    if ( definition.isNull() ) {
        LOG_WARNING("Definition is empty");
    }

    m_udpSocket = qobject_cast<QUdpSocket*>(peerSocket());

    if (!m_udpSocket) {
        LOG_ERROR("Preparation failed");
        return false;
    }

    m_udpSocket->setParent(this);

    // Signal for errors
    connect(m_udpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(handleError(QAbstractSocket::SocketError)));

    m_packetsReceived = 0;
    m_rec.clear();

    for(int i=0; i<definition->iterations; i++)
    {
        m_rec.append(QList<msg>());
    }

    return true;
}

bool PacketTrainsMP::stop()
{
    m_timeout.stop();
    return true;
}

ResultPtr PacketTrainsMP::result() const
{
    QVariantMap map;
    map.insert("sending_speed", listToVariant(m_sendSpeed));
    map.insert("receiving_speed", listToVariant(m_recvSpeed));

    return ResultPtr(new Result(QDateTime::currentDateTime(), map, QVariant()));
}
