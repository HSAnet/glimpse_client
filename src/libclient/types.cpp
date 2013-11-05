#include "types.h"

QVariant uuidToVariant(const QUuid &uuid)
{
    return uuid.toString().remove(QRegExp("[{}]"));
}
