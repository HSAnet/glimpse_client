#ifndef MEASUREMENT_H
#define MEASUREMENT_H

#include "measurementdefinition.h"
#include "../task/result.h"
#include "../traffic_budget_manager.h"

#include <QSharedPointer>

class Measurement;
typedef QSharedPointer<Measurement> MeasurementPtr;

class QAbstractSocket;
class NetworkManager;

class CLIENT_API Measurement : public QObject
{
    Q_OBJECT

public:
    Measurement(QObject *parent = 0);
    ~Measurement();

    void setPeerSocket(QAbstractSocket *peerSocket);
    QAbstractSocket *peerSocket() const;

    enum Status
    {
        Unknown,
        Running,
        Finished,
        Error
    };

    virtual Status status() const = 0;

    virtual bool prepare(NetworkManager *networkManager,
                         const MeasurementDefinitionPtr &measurementDefinition) = 0;

    virtual bool start() = 0;
    virtual bool stop() = 0;

    virtual Result result() const = 0;

    quint32 taskId() const;
    void setTaskId(const quint32 &taskId);

    QUuid getMeasurementUuid() const;
    void setMeasurementUuid(const QUuid &measurementUuid);

    QDateTime startDateTime() const;
    void setStartDateTime(const QDateTime &startDateTime);

    QDateTime endDateTime() const;
    void setEndDateTime(const QDateTime &endDateTime);

    QString errorString() const;

    QStringList resultHeader() const;
    void setResultHeader(const QStringList &resultHeader);

    TrafficBudgetManager *trafficBudgetManager() const;

signals:
    void started();
    void finished();
    void error(const QString &message);

protected:
    class Private;
    Private *d;

protected slots:
    void setErrorString(const QString &message);
};

#endif // MEASUREMENT_H
