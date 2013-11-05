#include "types.h"

QString uuidToString(const QUuid &uuid)
{
    return uuid.toString().remove(QRegExp("[{}]"));
}
