#ifndef GETSCHEDULERESPONSE_H
#define GETSCHEDULERESPONSE_H

#include "response.h"

class GetScheduleResponse : public Response
{
    Q_OBJECT

public:
    GetScheduleResponse(QObject *parent = 0)
    : Response(parent)
    {
    }

    bool fillFromVariant(const QVariantMap &variant);
    ScheduleDefinitionList tasks() const;

protected:
    TaskValidator m_validator;
    ScheduleDefinitionList m_tasks;
};

#endif // GETSCHEDULERESPONSE_H
