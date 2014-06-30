#include "getconfigrequest.h"

GetConfigRequest::GetConfigRequest(QObject *parent)
: Request(parent)
{
    setPath("/supervisor/api/v1/configuration/1/");
}

GetConfigRequest::~GetConfigRequest()
{
}

QVariant GetConfigRequest::toVariant() const
{
    return QVariant();
}
