#include "gettasksrequest.h"

GetTasksRequest::GetTasksRequest(QObject *parent)
: Request(parent)
{
}

GetTasksRequest::~GetTasksRequest()
{
}

QVariant GetTasksRequest::toVariant() const
{
    QVariantMap data;
    return data;
}
