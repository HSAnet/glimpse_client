#ifndef GETCONFIGREQUEST_H
#define GETCONFIGREQUEST_H

#include "request.h"

class CLIENT_API GetConfigRequest : public Request
{
    Q_OBJECT
    Q_CLASSINFO("path", "/supervisor/api/v1/configuration/1/")
    Q_CLASSINFO("method", "get")

public:
    GetConfigRequest(QObject* parent = 0);
    ~GetConfigRequest();

    QVariant toVariant() const;
};

#endif // GETCONFIGREQUEST_H
