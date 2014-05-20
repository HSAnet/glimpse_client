#ifndef GETTASKSREQUEST_H
#define GETTASKSREQUEST_H

#include "request.h"

class CLIENT_API GetTasksRequest : public Request
{
    Q_OBJECT
    Q_CLASSINFO("path", "/supervisor/api/v1/instruction/1/")
    Q_CLASSINFO("http_request_method", "get")
    Q_CLASSINFO("authentication_method", "apikey")

public:
    GetTasksRequest(QObject* parent = 0);
    ~GetTasksRequest();

    QVariant toVariant() const;
};

#endif // GETTASKSREQUEST_H
