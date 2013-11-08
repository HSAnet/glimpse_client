#include "types.h"

#include <QRegExp>

QString uuidToString(const QUuid &uuid)
{
    return uuid.toString().remove(QRegExp("[{}]"));
}
