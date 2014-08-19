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
    TestDefinitionList tasks() const;

protected:
    TaskValidator m_validator;
    TestDefinitionList m_tasks;
};

#endif // GETSCHEDULERESPONSE_H
