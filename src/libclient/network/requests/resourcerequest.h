#ifndef GETTASKSREQUEST_H
#define GETTASKSREQUEST_H

#include "request.h"

class CLIENT_API GetResourceRequest : public Request
{
    Q_OBJECT
    Q_CLASSINFO("http_request_method", "get")
    Q_CLASSINFO("authentication_method", "apikey")

public:
    GetResourceRequest(QObject *parent = 0);
    ~GetResourceRequest();

    QVariant toVariant() const;

    void addResourceId(const int &resourceId);
    QString path() const;

protected:
    class Private;
    friend class Private;
    Private *d;
};

#endif // GETTASKSREQUEST_H
