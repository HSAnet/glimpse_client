#include "uploadrequest.h"

class UploadRequest::Private
{
public:
    QVariantList resourceList;
};

UploadRequest::UploadRequest(QObject *parent)
: Request(parent)
, d(new Private)
{
}

QVariant UploadRequest::toVariant() const
{
    QVariantMap map;
    map.insert("device_id", deviceId());
    map.insert("data", d->resourceList);
    return map;
}

void UploadRequest::addResource(const QVariant &resource)
{
    d->resourceList.append(resource);
}

void UploadRequest::clearResources()
{
    d->resourceList.clear();
}
