#include "resourcerequest.h"
#include <QStringList>

class GetResourceRequest::Private
{
public:
    QStringList resourceIds;
    QVariantMap data;
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
    d->data.insert("device_id", deviceId());
    return d->data;
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

void GetResourceRequest::addData(const QVariantMap &data)
{
    QMapIterator<QString, QVariant> i(data);
    while (i.hasNext())
    {
        i.next();
        d->data.insert(i.key(), i.value());
    }
}

QStringList GetResourceRequest::resourceIds()
{
    return d->resourceIds;
}
