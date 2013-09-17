#ifndef CONNECTIONTESTER_H
#define CONNECTIONTESTER_H

#include "export.h"
#include <QObject>
#include <QAbstractListModel>

class CLIENT_API ConnectionTester : public QObject
{
    Q_OBJECT
    Q_ENUMS(ResultType TestType)
    Q_PROPERTY(bool running READ isRunning NOTIFY runningChanged)

public:
    explicit ConnectionTester(QObject *parent = 0);
    ~ConnectionTester();

    enum ResultType
    {
        Online, // Everything works fine
        NoConnection, // Devices up but now connection
        Offline // No device is up
    };

    enum TestType
    {
        ActiveInterface,
        // If interface is up
        DefaultGateway,
        DefaultDns,
        PingDefaultGateway,
        PingGoogleDnsServer,
        PingGoogleDomain
    };

    bool isRunning() const;

    Q_INVOKABLE void start();
    ResultType result() const;

    Q_INVOKABLE bool checkOnline();
    Q_INVOKABLE QString findDefaultGateway();
    Q_INVOKABLE QString findDefaultDNS();
    Q_INVOKABLE bool canPingGateway();
    Q_INVOKABLE bool canPingGoogleDnsServer();
    Q_INVOKABLE bool canPingGoogleDomain();

signals:
    void runningChanged();

    void started();
    void checkStarted(ConnectionTester::TestType testType);
    void checkFinished(ConnectionTester::TestType testType, bool success, const QVariant& result);
    void finished();

protected:
    bool canPing(TestType testType, const QString& host);

private:
    class Private;
    Private* d;
    friend class Private;
};

class ConnectionTesterModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(ConnectionTester* connectionTester READ connectionTester WRITE setConnectionTester NOTIFY connectionTesterChanged)

public:
    ConnectionTesterModel(QObject* parent = 0);
    ~ConnectionTesterModel();

    enum Roles
    {
        TestNameRole = Qt::UserRole + 1,
        TestTypeRole,
        TestFinishedRole,
        TestResultRole,
        TestSuccessRole
    };

    void setConnectionTester(ConnectionTester* connectionTester);
    ConnectionTester* connectionTester() const;

    Q_INVOKABLE QVariant result() const;

    // QAbstractListModel overrides
    QHash<int, QByteArray> roleNames() const;
    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;

signals:
    void connectionTesterChanged(ConnectionTester* connectionTester);

private slots:
    void onStarted();
    void onCheckStarted(ConnectionTester::TestType testType);
    void onCheckFinished(ConnectionTester::TestType testType, bool success, const QVariant& result);
    void onFinished();

protected:
    struct RowData {
        ConnectionTester::TestType testType;
        bool success;
        bool finished;
        QVariant result;
    };

protected:
    ConnectionTester* m_connectionTester;
    QList<RowData> m_rows;
};

Q_DECLARE_METATYPE(ConnectionTester::TestType)

#endif // CONNECTIONTESTER_H
