#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "../timing/timing.h"
#include "../timing/timingfactory.h" // TODO: Remove

#include <QUuid>

class CLIENT_API TestDefinition : public Serializable
{
public:
    TestDefinition(const QUuid& id, const QString& name, const TimingPtr& timing)
        : id(id)
        , name(name)
        , timing(timing)
    {
    }

    QUuid id;
    QString name;
    TimingPtr timing;

    // Serializable interface
    QVariant toVariant() const {
        QVariantMap hash;
        hash.insert("id", id);
        hash.insert("name", name);
        hash.insert("timing", timing->toVariant());
        return hash;
    }

    static TestDefinition fromVariant(const QVariant &variant) {
        QVariantMap hash = variant.toMap();

        return TestDefinition(hash.value("id").toUuid(),
                              hash.value("name").toString(),
                              TimingFactory::timingFromVariant(hash.value("timing")));
    }
};
typedef QList<TestDefinition> TestDefinitionList;

class CLIENT_API Scheduler : public QObject
{
    Q_OBJECT

public:
    Scheduler();
    ~Scheduler();

    TestDefinitionList tests() const;

    void enqueue(const TestDefinition& testDefinition);
    void dequeue();

    void execute(const TestDefinition& testDefinition);

signals:
    void testAdded(const TestDefinition& test, int position);
    void testRemoved(const TestDefinition& test, int position);
    void testMoved(const TestDefinition& test, int from, int to);

protected:
    class Private;
    Private* d;
};

#endif // SCHEDULER_H
