#ifndef POSTREQUEST_H
#define POSTREQUEST_H

#include "request.h"

class CLIENT_API PostRequest : public Request
{
    Q_OBJECT
    Q_CLASSINFO("http_request_method", "post")


public:
    PostRequest(QObject *parent = 0);
    ~PostRequest();

    QVariant toVariant() const;

    void clear();
    void setData(const QVariant &data);

signals:

protected:
    class Private;
    friend class Private;
    Private *d;
};

#endif // POSTREQUEST_H
