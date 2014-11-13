#include "resourcerequest.h"
#include <QStringList>

class GetResourceRequest::Private
{
public:
    QStringList resourceIds;
};

GetResourceRequest::GetResourceRequest(QObject *parent)
: Request(parent)
, d(new Private)
{
}

GetResourceRequest::~GetResourceRequest()
{
    delete d;
}

QVariant GetResourceRequest::toVariant() const
{
    QVariantMap data;
    data.insert("device_id", deviceId());
    return data;
}

void GetResourceRequest::addResourceId(const int &resourceId)
{
    d->resourceIds.append(QString::number(resourceId));
}

void GetResourceRequest::clearResourceIds()
{
    d->resourceIds.clear();
}

QString GetResourceRequest::path() const
{
    if (d->resourceIds.isEmpty())
    {
        return Request::path();
    }
    else
    {
        return QString("%1set/%2/").arg(Request::path()).arg(d->resourceIds.join(';'));
    }
}

QStringList GetResourceRequest::resourceIds()
{
    return d->resourceIds;
}
