#ifndef TASK_H
#define TASK_H

#include "../ident.h"
#include "../timing/timing.h"
#include "../measurement/measurementdefinition.h"
#include "../precondition.h"

#include <QUuid>
#include <QSharedDataPointer>

class ScheduleDefinition;
typedef QList<ScheduleDefinition> ScheduleDefinitionList;

class CLIENT_API ScheduleDefinition : public Serializable
{
public:
    ScheduleDefinition();
    ScheduleDefinition(const ScheduleDefinition &other);
    ScheduleDefinition(const ScheduleId &id, const TaskId &taskId, const QString &name, const TimingPtr &timing, const QVariant &measurementDefinition, const Precondition &precondition);
    ~ScheduleDefinition();

    ScheduleDefinition &operator=(const ScheduleDefinition &rhs);

    bool isNull() const;

    void setId(const ScheduleId &id);
    ScheduleId id() const;

    TaskId taskId() const;

    void setName(const QString &name);
    QString name() const;

    void setTiming(const TimingPtr &timing);
    TimingPtr timing() const;

    void setMeasurementDefinition(const QVariant &measurementDefinition);
    QVariant measurementDefinition() const;

    void setPrecondition(const Precondition &precondition);
    Precondition precondition() const;

    // Storage
    static ScheduleDefinition fromVariant(const QVariant &variant);

    // Serializable interface
    QVariant toVariant() const;

private:
    QSharedDataPointer<class TaskData> d;
};

Q_DECLARE_METATYPE(ScheduleDefinition)
Q_DECLARE_TYPEINFO(ScheduleDefinition, Q_MOVABLE_TYPE);
Q_DECLARE_METATYPE(ScheduleDefinitionList)

#endif // TASK_H
