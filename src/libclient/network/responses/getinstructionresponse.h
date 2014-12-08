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
    QList<ScheduleId> removeScheduleIds() const;
    bool fillFromVariant(const QVariantMap &variant);

protected:
    QList<ScheduleId> m_taskIds;
    QList<ScheduleId> m_scheduleIds;
    QList<ScheduleId> m_removeScheduleIds;
};


#endif // GETINSTRUCTIONRESPONSE_H
