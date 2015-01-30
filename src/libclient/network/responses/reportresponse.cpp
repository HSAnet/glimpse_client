#include "reportresponse.h"

bool ReportResponse::fillFromVariant(const QVariantMap &variant)
{
    this->taskIds.clear();

    QVariantList taskIds = variant.value("tasks").toList();

    foreach (const QVariant &id, taskIds)
    {
        this->taskIds.append(TaskId(id.toInt()));
    }

    return true;
}

void ReportResponse::finished()
{

}
