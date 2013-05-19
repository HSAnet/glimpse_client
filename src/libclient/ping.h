#ifndef PING_H
#define PING_H

#include <QObject>

class Ping : public QObject
{
    Q_OBJECT
    Q_ENUMS(Status)
    Q_PROPERTY(int status READ status NOTIFY statusChanged)
    Q_PROPERTY(int exitStatus READ exitStatus NOTIFY statusChanged)
    Q_PROPERTY(QString host READ host WRITE setHost NOTIFY hostChanged)
    Q_PROPERTY(int amount READ amount WRITE setAmount NOTIFY amountChanged)
    Q_PROPERTY(int timeout READ timeout WRITE setTimeout NOTIFY timeoutChanged)
    Q_PROPERTY(QList<int> pingTime READ pingTime NOTIFY ping)
    Q_PROPERTY(int averagePingTime READ averagePingTime NOTIFY ping)

public:
    explicit Ping(QObject *parent = 0);
    ~Ping();

    enum Status
    {
        NotRunning,
        Running
    };

    Status status() const;

    enum ExitStatus
    {
        Normal,
        Failed
    };

    ExitStatus exitStatus() const;

    void setHost(const QString& host);
    QString host() const;

    void setAmount(int amount);
    int amount() const;

    void setTimeout(int timeout);
    int timeout() const;

    QList<int> pingTime() const;
    int averagePingTime() const;

public slots:
    void start();
    void stop();

signals:
    void hostChanged(const QString& host);
    void amountChanged(int amount);
    void timeoutChanged(int timeout);

    void started();
    void stopped();
    void statusChanged(Status status);

    void ping(int time);

protected:
    class Private;
    friend class Private;
    Private* d;
};

#endif // PING_H
