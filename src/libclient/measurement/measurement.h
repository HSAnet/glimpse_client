#ifndef MEASUREMENT_H
#define MEASUREMENT_H

#include "measurementdefinition.h"
#include "../task/result.h"

#include <QSharedPointer>

class Measurement;
typedef QSharedPointer<Measurement> MeasurementPtr;

class QAbstractSocket;
class NetworkManager;

class CLIENT_API Measurement : public QObject
{
    Q_OBJECT

public:
    Measurement(QObject* parent = 0);
    ~Measurement();

    void setPeerSocket(QAbstractSocket* peerSocket);
    QAbstractSocket* peerSocket() const;

    enum Status
    {
        Unknown,
        Running,
        Finished,
        Error
    };

    virtual Status status() const = 0;

    virtual bool prepare(NetworkManager* networkManager,
                         const MeasurementDefinitionPtr& measurementDefinition) = 0;

    virtual bool start() = 0;
    virtual bool stop() = 0;

    virtual ResultPtr result() const = 0;

    QUuid taskId() const;
    void setTaskId(const QUuid &taskId);

    QUuid getMeasurementUuid() const;
    void setMeasurementUuid(const QUuid &measurementUuid);

signals:
    void started();
    void finished();

protected:
    class Private;
    Private* d;
};

#endif // MEASUREMENT_H
