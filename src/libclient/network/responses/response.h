#ifndef RESPONSE_H
#define RESPONSE_H

#include <QObject>
#include <QVariantMap>

#include "timing/timing.h"
#include "channel.h"
#include "task/scheduledefinition.h"
#include "task/taskvalidator.h"


class CLIENT_API Response : public QObject
{
    Q_OBJECT

public:
    explicit Response(QObject *parent = 0);
    ~Response();

    virtual bool fillFromVariant(const QVariantMap &variant) = 0;
    virtual void finished();
};









#endif // RESPONSE_H
