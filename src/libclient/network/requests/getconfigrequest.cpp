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
    QVariantMap map;
    map.insert("session_id", sessionId());
    return map;
}
