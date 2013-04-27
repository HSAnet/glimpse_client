#ifndef CONNECTIONTESTER_H
#define CONNECTIONTESTER_H

#include <QObject>

class ConnectionTester : public QObject
{
    Q_OBJECT
    Q_ENUMS(MessageType)

public:
    explicit ConnectionTester(QObject *parent = 0);
    ~ConnectionTester();

    enum MessageType
    {
        Info,
        Warning,
        Error
    };

    enum ResultType
    {
        Online, // Everything works fine
        NoConnection, // Devices up but now connection
        Offline // No device is up
    };

    void start();
    ResultType result() const;

signals:
    void message(const QString& message, MessageType type);
    void finished();

private:
    class Private;
    Private* d;
    friend class Private;
};

#endif // CONNECTIONTESTER_H
