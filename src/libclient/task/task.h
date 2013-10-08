#ifndef TASK_H
#define TASK_H

#include "../timing/timing.h"

#include <QUuid>

class TestDefinition;
typedef QSharedPointer<TestDefinition> TestDefinitionPtr;
typedef QList<TestDefinitionPtr> TestDefinitionList;

class CLIENT_API TestDefinition : public Serializable
{
public:
    TestDefinition(const QUuid& id, const QString& name, const TimingPtr& timing);
    ~TestDefinition();

    // Storage
    static TestDefinitionPtr fromVariant(const QVariant &variant);

    // Getters
    QUuid id() const;
    QString name() const;
    TimingPtr timing() const;

    // Serializable interface
    QVariant toVariant() const;

protected:
    class Private;
    Private* d;
};

#endif // TASK_H
