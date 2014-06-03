#include "getconfigrequest.h"

GetConfigRequest::GetConfigRequest(QObject *parent)
: Request(parent)
{
}

GetConfigRequest::~GetConfigRequest()
{
}

QVariant GetConfigRequest::toVariant() const
{
    return QVariant();
}
