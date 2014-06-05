#ifndef MANUALREQUEST_H
#define MANUALREQUEST_H

#include "request.h"

class CLIENT_API ManualRequest : public Request
{
    Q_OBJECT
    Q_CLASSINFO("path", "/manualrequest")

public:
    ManualRequest(QObject *parent = 0);
    ~ManualRequest();

    QVariant toVariant() const;

protected:
    class Private;
    Private *d;
};

#endif // MANUALREQUEST_H
