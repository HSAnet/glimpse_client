#ifndef REPORTRESPONSE_H
#define REPORTRESPONSE_H

#include "response.h"

class ReportResponse : public Response
{
    Q_OBJECT

public:
    ReportResponse(QObject *parent = 0)
    : Response(parent)
    {
    }

    // Response interface
    bool fillFromVariant(const QVariantMap &variant);
    void finished();

    QList<TaskId> taskIds;
};
#endif // REPORTRESPONSE_H
