#ifndef TASK_H
#define TASK_H

#include "ident.h"
#include "serializable.h"
#include "measurement/measurementdefinition.h"

#include <QSharedDataPointer>

class Task;
typedef QList<Task> TaskList;

class CLIENT_API Task : public Serializable
{
public:
    Task();
    Task(const Task &other);
    Task(const TaskId &id, const QString &method, const QVariant &measurementDefinition);
    ~Task();

    Task &operator=(const Task &task);

    bool isNull() const;

    void setId(const TaskId &id);
    TaskId id() const;

    void setMethod(const QString &method);
    QString method() const;

    void setMeasurementDefinition(const QVariant &measurementDefinition);
    QVariant measurementDefinition() const;

    static Task fromVariant(const QVariant &variant);
    QVariant toVariant() const;

    inline bool operator==(const Task &b) const
    {
        return (this->id() == b.id());
    }

private:
    class Private;
    QSharedDataPointer<Private> d;
};

Q_DECLARE_METATYPE(Task)
Q_DECLARE_TYPEINFO(Task, Q_MOVABLE_TYPE);
Q_DECLARE_METATYPE(TaskList)

#endif // TASK_H
