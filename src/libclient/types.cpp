#include "types.h"

#include <QCryptographicHash>
#include <QRegExp>

QString uuidToString(const QUuid &uuid)
{
    return uuid.toString().remove(QRegExp("[{}]"));
}

QString userIdToHash(const QString &userId)
{
    QCryptographicHash hash(QCryptographicHash::Sha224);
    hash.addData(userId.toUtf8());

    return hash.result().toHex().left(30);
}
