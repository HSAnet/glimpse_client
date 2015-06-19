#include "resultscheduler.h"

class ResultScheduler::Private
{
public:
    ExtResultList results;
};

ResultScheduler::ResultScheduler()
: d(new Private)
{

}

ResultScheduler::~ResultScheduler()
{
    delete d;
}

ExtResultList ResultScheduler::results() const
{
    return d->results;
}

void ResultScheduler::addResult(const TaskId &id, const Result &result)
{
    for (int i = 0; i < d->results.size(); i++)
    {
        if (d->results.at(i).value("task_id") == id.toInt())
        {
            QVariantMap map = d->results.at(i);
            QVariantList list = map.value("results").toList();

            list << result.toVariantStripped();
            map["results"] = list;

            d->results[i] = map;
            emit resultModified(id, result);

            return;
        }
    }

    QVariantMap map;
    QVariantList list;

    list << result.toVariantStripped();

    map.insert("task_id", id.toInt());
    map.insert("results", list);

    d->results.append(map);
    emit resultAdded(id, result);
}
