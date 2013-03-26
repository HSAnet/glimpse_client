#ifndef NEGOTIATOR_H
#define NEGOTIATOR_H

#include <QUdpSocket>

class Negotiator : public QObject
{
    Q_OBJECT

public:
    explicit Negotiator(QObject *parent = 0);
    ~Negotiator();

    /**
     * @param managerSocket Must be a connected socket
     */
    void setManagerSocket(QAbstractSocket *managerSocket);
    QAbstractSocket *managerSocket() const;

    void sendRequest(const QHostAddress& address, quint16 port);

signals:
    void finished();
    void error();

protected:
    class Private;
    friend class Private;
    Private* d;
};

#endif // NEGOTIATOR_H
