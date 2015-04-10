#ifndef SPECIFICATIONRESPONSE_H
#define SPECIFICATIONRESPONSE_H

#include "response.h"

class SpecificationResponse : public Response
{
     Q_OBJECT

public:
    SpecificationResponse(QObject *parent = 0);
    ~SpecificationResponse();

    bool fillFromVariant(const QVariantMap &variant);

    ScheduleDefinitionList tasks();

protected:
    class Private;
    friend class Private;
    Private *d;
};

#endif // SPECIFICATIONRESPONSE_H
