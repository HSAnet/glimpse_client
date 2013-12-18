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
{
}

bool PacketTrainsMP::start()
{
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
    struct timespec timestamp;
    struct msg *message;
    quint16 iter;

    while(m_udpSocket->hasPendingDatagrams())
    {
        // get time first
        clock_gettime(CLOCK_MONOTONIC_RAW, &timestamp); // TODO try QTime

        QByteArray buffer;
        buffer.resize(m_udpSocket->pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;

        m_udpSocket->readDatagram(buffer.data(), buffer.size(), &sender, &senderPort);
        message = reinterpret_cast<msg*>(buffer.data());
        message->type = (msgtype) ntohs(message->type);

        switch(message->type)
        {
        case MSG_CTRL: // new train
            m_packetsReceived = 0;

            m_rec.clear();

            for(int i=0; i<ntohs(message->data.c.iter); i++)
            {
                m_rec.append(QList<msrmnt>());
            }
            // TODO answer ack

            m_timer.start();
            break;
        case MSG_MSRMNT: // train data
            message->data.m.rtime = timestamp;
            message->data.m.r_id = m_packetsReceived++;

            iter = ntohs(message->data.m.iter);

            if(iter < m_rec.size())
            {
                m_rec[iter].append(message->data.m);
            }
            else
            {
                // TODO error case
                qDebug() << "error case";
            }

            m_timer.start();
            break;
        default:
            qDebug() << "Error: Unkown message type";
        }
    }
}

void PacketTrainsMP::eval()
{
    foreach(const QList<msrmnt> &l, m_rec)
    {
        quint64 ts_otime[2] = {0}, ts_rtime[2] = {2};
        double srate = 0, rrate = 0;

        for (int i = 0; i < l.size(); i++) {
            if (l[i].id != l[i].r_id) {
                    qDebug() <<"packages out of order";
                    //return;
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
            m_recvSpeed.append(rrate / 1024 * 8);
            m_sendSpeed.append(srate / 1024 * 8);
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

    // FIXME: This socket should come from NetworkManager
    m_udpSocket = new QUdpSocket(this);
    if (!(m_udpSocket->bind(definition->port))) {
        LOG_ERROR("Preparation failed (binding port failed)");
        return false;
    }

    // FIXME: We need to know the remote address here
    m_udpSocket->writeDatagram(QByteArray(), QHostAddress("192.168.178.38"), definition->port);

    if (!m_udpSocket) {
        LOG_ERROR("Preparation failed");
        return false;
    }

    m_udpSocket->setParent(this);

    // Signal for errors
    connect(m_udpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(handleError(QAbstractSocket::SocketError)));

    return true;
}

bool PacketTrainsMP::stop()
{
    return true;
}

ResultPtr PacketTrainsMP::result() const
{
    QVariantMap map;
    map.insert("sending_speed", listToVariant(m_sendSpeed));
    map.insert("receiving_speed", listToVariant(m_recvSpeed));

    return ResultPtr(new Result(QDateTime::currentDateTime(), map, QVariant()));
}
