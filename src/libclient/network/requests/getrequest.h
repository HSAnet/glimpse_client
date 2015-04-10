#ifndef GETREQUEST_H
#define GETREQUEST_H

#include "request.h"

class GetRequest : public Request
{
    Q_OBJECT
    Q_CLASSINFO("http_request_method", "get")


public:
    GetRequest(QObject *parent = 0);
    ~GetRequest();

    QVariant toVariant() const;
};

#endif // GETREQUEST_H
