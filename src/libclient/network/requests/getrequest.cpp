#include "getrequest.h"

GetRequest::GetRequest(QObject *parent)
    : Request(parent)
{

}

GetRequest::~GetRequest()
{

}

QVariant GetRequest::toVariant() const
{
    return QVariant();
}

