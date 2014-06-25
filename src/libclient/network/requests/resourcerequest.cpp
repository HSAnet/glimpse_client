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
    return QVariantMap();
}

void GetResourceRequest::addResourceId(const int &resourceId)
{
    d->resourceIds.append(QString::number(resourceId));
}

QString GetResourceRequest::path() const
{
    if (d->resourceIds.isEmpty())
    {
        return Request::path();
    }
    else
    {
        return QString("%1set/%2").arg(Request::path()).arg(d->resourceIds.join(';'));
    }
}
