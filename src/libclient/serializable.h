#ifndef SERIALIZABLE_H
#define SERIALIZABLE_H

#include "export.h"

#include <QVariant>

class CLIENT_API Serializable
{
public:
    virtual ~Serializable() {}
    virtual QVariant toVariant() const = 0;
};


#endif // SERIALIZABLE_H
