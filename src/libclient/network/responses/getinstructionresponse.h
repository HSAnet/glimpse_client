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

    QList<int> taskIds() const;
    QList<int> scheduleIds() const;
    bool fillFromVariant(const QVariantMap &variant);

protected:
    QList<int> m_taskIds;
    QList<int> m_scheduleIds;
};


#endif // GETINSTRUCTIONRESPONSE_H
