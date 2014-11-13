#ifndef GETINSTRUCTIONRESPONSE_H
#define GETINSTRUCTIONRESPONSE_H

#include "response.h"

class GetInstructionResponse : public Response
{
    Q_OBJECT

public:
    GetInstructionResponse(QObject *parent = 0)
    : Response(parent)
    {
    }

    QList<ScheduleId> taskIds() const;
    QList<ScheduleId> scheduleIds() const;
    bool fillFromVariant(const QVariantMap &variant);

protected:
    QList<ScheduleId> m_taskIds;
    QList<ScheduleId> m_scheduleIds;
};


#endif // GETINSTRUCTIONRESPONSE_H
