#include "postrequest.h"

class PostRequest::Private
{
public:
    QVariantMap data;
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

    QVariantList contents;
    contents.append(d->data);
    envelope.insert("contents",contents);
    return envelope;
}

void PostRequest::addData(const QVariantMap &data)
{
    QMapIterator<QString, QVariant> i(data);
    while (i.hasNext())
    {
        i.next();
        d->data.insert(i.key(), i.value());
    }
}
