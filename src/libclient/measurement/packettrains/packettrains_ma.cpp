#include "packettrains_ma.h"
#include <QUdpSocket>
#include <QElapsedTimer>
#include <iostream>
using namespace std;
#include <iomanip>
#include "../../log/logger.h"
#include "../../network/networkmanager.h"

#ifdef Q_OS_WIN
#include <WinSock2.h>
#else
#include <arpa/inet.h>
#endif

#include <chrono>
#include <thread>

LOGGER(PacketTrainsMA);


PacketTrainsMA::PacketTrainsMA()
{
}

bool PacketTrainsMA::start()
{
    QByteArray buffer;
    buffer.resize(definition->packetSize);

    struct msg* message = reinterpret_cast<msg*>(buffer.data());

    // calculate disperson
    quint64* disp = new quint64[definition->iterations];
    quint64 R_MIN = 10485760;
    quint64 R_MAX = 262144000;
    qint64 delay = 200000000;

    for(int i = 0; i < definition->iterations; i++)
    {
        disp[i] = (quint64) (definition->packetSize * 1000000000.0 / ((R_MAX - R_MIN) / definition->iterations * i + R_MIN)); // Linear Rate
    }

    QElapsedTimer timer;
    timer.start();

    // send trains
    for(int i = 0; i < definition->trainLength * definition->iterations; i++)
    {
        message->iter = htons(i / definition->trainLength);
        message->id = i % definition->trainLength;

        message->otime = timer.nsecsElapsed();

        m_udpSocket->writeDatagram(buffer, QHostAddress(definition->host), definition->port);

        std::this_thread::sleep_for(std::chrono::nanoseconds(i / definition->trainLength));
        if(i % definition->trainLength == definition->trainLength -1)
        {
            std::this_thread::sleep_for(std::chrono::nanoseconds(delay));
        }
    }

    delete disp;

    emit finished();
    return true;
}

void PacketTrainsMA::handleError(QAbstractSocket::SocketError socketError)
{
    if(socketError == QAbstractSocket::RemoteHostClosedError)
        return;

    QAbstractSocket* socket = qobject_cast<QAbstractSocket*>(sender());
    cout<<"Socket Error: "<<socket->errorString().toStdString()<<endl;
}


Measurement::Status PacketTrainsMA::status() const
{
    return Unknown;
}

bool PacketTrainsMA::prepare(NetworkManager *networkManager, const MeasurementDefinitionPtr &measurementDefinition)
{
    definition = measurementDefinition.dynamicCast<PacketTrainsDefinition>();
    if ( definition.isNull() ) {
        LOG_WARNING("Definition is empty");
    }

    QString hostname = QString("%1:%2").arg(definition->host).arg(definition->port);

    m_udpSocket = qobject_cast<QUdpSocket*>(networkManager->establishConnection(hostname, "packettrains_mp", definition->toVariant(), NetworkManager::UdpSocket));
    if (!m_udpSocket) {
        LOG_ERROR("Preparation failed");
        return false;
    }

    m_udpSocket->setParent(this);

    // Signal for errors
    connect(m_udpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(handleError(QAbstractSocket::SocketError)));

    return true;
}

bool PacketTrainsMA::stop()
{
    return true;
}

ResultPtr PacketTrainsMA::result() const
{
    return ResultPtr();
}
