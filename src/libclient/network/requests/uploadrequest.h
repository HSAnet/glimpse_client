#ifndef UPLOADREQUEST_H
#define UPLOADREQUEST_H

#include <QVariant>
#include "request.h"

class UploadRequest : public Request
{
    Q_OBJECT
    Q_CLASSINFO("http_request_method", "post")
    Q_CLASSINFO("authentication_method", "apikey")

public:
    UploadRequest(QObject *parent = 0);

    QVariant toVariant() const;

    void addResource(const QVariant &resource);
    void clearResources();

protected:
    class Private;
    friend class Private;
    Private *d;
};

#endif // UPLOADREQUEST_H
