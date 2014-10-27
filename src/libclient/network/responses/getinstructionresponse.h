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

    QList<TaskId> taskIds() const;
    QList<TaskId> scheduleIds() const;
    bool fillFromVariant(const QVariantMap &variant);

protected:
    QList<TaskId> m_taskIds;
    QList<TaskId> m_scheduleIds;
};


#endif // GETINSTRUCTIONRESPONSE_H
