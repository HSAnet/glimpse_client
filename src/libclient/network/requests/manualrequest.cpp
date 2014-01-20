#include "manualrequest.h"

class ManualRequest::Private
{
public:
};

ManualRequest::ManualRequest(QObject *parent)
: Request(parent)
, d(new Private)
{
}

ManualRequest::~ManualRequest()
{
    delete d;
}

QVariant ManualRequest::toVariant() const
{
    QVariantMap data;
    data.insert("device_id", deviceId());
    return data;
}
