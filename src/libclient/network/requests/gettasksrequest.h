#ifndef GETTASKSREQUEST_H
#define GETTASKSREQUEST_H

#include "request.h"

class CLIENT_API GetTasksRequest : public Request
{
    Q_OBJECT
    Q_CLASSINFO("path", "/get_tasks")

public:
    GetTasksRequest(QObject* parent = 0);
    ~GetTasksRequest();

    QVariant toVariant() const;
};

#endif // GETTASKSREQUEST_H
