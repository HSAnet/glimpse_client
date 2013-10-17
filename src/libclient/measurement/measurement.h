#ifndef MEASUREMENT_H
#define MEASUREMENT_H

#include "measurementdefinition.h"
#include "../network/networkmanager.h"
#include "../task/result.h"

#include <QSharedPointer>

class Measurement;
typedef QSharedPointer<Measurement> MeasurementPtr;

class CLIENT_API Measurement : public QObject
{
    Q_OBJECT

public:
    Measurement(QObject* parent = 0);
    ~Measurement();

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

signals:
    void started();
    void finished();
};

#endif // MEASUREMENT_H
