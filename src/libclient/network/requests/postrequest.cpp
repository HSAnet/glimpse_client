#include "postrequest.h"

class PostRequest::Private
{
public:
    //QVariantMap data;
    QVariantList data;
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
    QVariantMap envelope;

    envelope.insert("envelope", "message");
    envelope.insert("version", 0);

    envelope.insert("contents", d->data);
    return envelope;
}

void PostRequest::addData(const QVariantMap &data)
{
    d->data.append(data);
    /*
    QMapIterator<QString, QVariant> i(data);
    while (i.hasNext())
    {
        i.next();
        d->data.insert(i.key(), i.value());
    }*/
}
