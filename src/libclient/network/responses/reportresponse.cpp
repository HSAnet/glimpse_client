#include "reportresponse.h"

bool ReportResponse::fillFromVariant(const QVariantMap &variant)
{

    // TODO: new format
    this->taskIds.clear();

    QVariantList taskIds = variant.value("tasks").toList();

    foreach (const QVariant &id, taskIds)
    {
        this->taskIds.append(id.toInt());
    }

    return true;
}

void ReportResponse::finished()
{

}
