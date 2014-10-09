#ifndef TASK_H
#define TASK_H

#include "../ident.h"
#include "../timing/timing.h"
#include "../measurement/measurementdefinition.h"
#include "../precondition.h"

#include <QUuid>
#include <QSharedDataPointer>

class TestDefinition;
typedef QList<TestDefinition> TestDefinitionList;

class CLIENT_API TestDefinition : public Serializable
{
public:
    TestDefinition();
    TestDefinition(const TestDefinition &other);
    TestDefinition(const TaskId &id, const QString &name, const TimingPtr &timing, const QVariant &measurementDefinition, const Precondition &precondition);
    ~TestDefinition();

    TestDefinition &operator=(const TestDefinition &rhs);

    bool isNull() const;

    void setId(const TaskId &id);
    TaskId id() const;

    void setName(const QString &name);
    QString name() const;

    void setTiming(const TimingPtr &timing);
    TimingPtr timing() const;

    void setMeasurementDefinition(const QVariant &measurementDefinition);
    QVariant measurementDefinition() const;

    void setPrecondition(const Precondition &precondition);
    Precondition precondition() const;

    // Storage
    static TestDefinition fromVariant(const QVariant &variant);

    // Serializable interface
    QVariant toVariant() const;

private:
    QSharedDataPointer<class TaskData> d;
};

Q_DECLARE_METATYPE(TestDefinition)
Q_DECLARE_TYPEINFO(TestDefinition, Q_MOVABLE_TYPE);
Q_DECLARE_METATYPE(TestDefinitionList)

#endif // TASK_H
