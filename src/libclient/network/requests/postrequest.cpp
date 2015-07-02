#include "postrequest.h"

class PostRequest::Private
{
public:
    //QVariantMap data;
    QVariant data;
};

PostRequest::PostRequest(QObject *parent)
    : Request(parent)
    , d(new Private)
{

}

PostRequest::~PostRequest()
{
    delete d;
}

QVariant PostRequest::toVariant() const
{
    return d->data;
}

void PostRequest::clear()
{
    d->data.clear();
}

void PostRequest::setData(const QVariant &data)
{
    d->data = data;
    /*
    QMapIterator<QString, QVariant> i(data);
    while (i.hasNext())
    {
        i.next();
        d->data.insert(i.key(), i.value());
    }*/
}
