#ifndef TASK_H
#define TASK_H

#include "../timing/timing.h"
#include "../measurement/measurementdefinition.h"

#include <QUuid>
#include <QSharedDataPointer>

class TestDefinition;
typedef QList<TestDefinition> TestDefinitionList;


class TaskData : public QSharedData
{
public:
    TaskData()
    {
    }

    TaskData(const TaskData &other)
    : QSharedData(other)
    , id(other.id)
    , name(other.name)
    , timing(other.timing)
    , measurementDefinition(other.measurementDefinition)
    {
    }

    QUuid id;
    QString name;
    TimingPtr timing;
    QVariant measurementDefinition;
};


class CLIENT_API TestDefinition : public Serializable
{
public:
    TestDefinition();
    TestDefinition(const TestDefinition &other);
    TestDefinition(const QUuid &id, const QString &name, const TimingPtr &timing, const QVariant &measurementDefinition);

    bool isNull() const;

    void setId(const QUuid &id);
    QUuid id() const;

    void setName(const QString &name);
    QString name() const;

    void setTiming(const TimingPtr &timing);
    TimingPtr timing() const;

    void setMeasurementDefinition(const QVariant &measurementDefinition);
    QVariant measurementDefinition() const;

    // Storage
    static TestDefinition fromVariant(const QVariant &variant);

    // Serializable interface
    QVariant toVariant() const;

private:
    QSharedDataPointer<TaskData> d;
};

Q_DECLARE_METATYPE(TestDefinition)
Q_DECLARE_TYPEINFO(TestDefinition, Q_MOVABLE_TYPE);
Q_DECLARE_METATYPE(TestDefinitionList)

#endif // TASK_H
