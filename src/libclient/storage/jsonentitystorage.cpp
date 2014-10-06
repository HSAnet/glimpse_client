#include "jsonentitystorage.h"

#include <QMetaType>
#include <QVariantList>
#include <QStack>
#include <QDebug>

namespace
{
    QVariant realVariant(const QVariant &variant)
    {
        if (variant.type() != QVariant::UserType)
        {
            return variant;
        }

        int userType = variant.userType();

        if (QMetaType::hasRegisteredConverterFunction(userType, qMetaTypeId<QString>()))
        {
            return qvariant_cast<QString>(variant);
        }

        if (QMetaType::hasRegisteredConverterFunction(userType, qMetaTypeId<int>()))
        {
            return qvariant_cast<int>(variant);
        }

        if (QMetaType::hasRegisteredConverterFunction(userType, qMetaTypeId<double>()))
        {
            return qvariant_cast<double>(variant);
        }

        if (QMetaType::hasRegisteredConverterFunction(userType, qMetaTypeId<float>()))
        {
            return qvariant_cast<float>(variant);
        }

        qWarning("realVariant: Unable to find a real variant type");
        return variant;
    }
}

void bsonAppend(bson *b, const QVariant &variant, const QString &name)
{
    QVariant localVariant = realVariant(variant);

    switch(localVariant.type())
    {
    case QVariant::Map:
        {
            bson_append_start_object(b, name.toLatin1());

            QVariantMap map = localVariant.toMap();
            QMapIterator<QString, QVariant> iter(map);
            while (iter.hasNext())
            {
                iter.next();

                bsonAppend(b, iter.value(), iter.key());
            }

            bson_append_finish_object(b);
        }
        break;

    case QVariant::StringList:
    case QVariant::List:
        {
            bson_append_start_array(b, name.toLatin1());

            QVariantList list = localVariant.toList();
            foreach(const QVariant &subvariant, list)
            {
                bsonAppend(b, subvariant, name);
            }

            bson_append_finish_array(b);
        }
        break;

    case QVariant::ByteArray:
    case QVariant::String:
        bson_append_string(b, name.toLatin1(), localVariant.toString().toLatin1());
        break;

    case QVariant::Int:
        bson_append_int(b, name.toLatin1(), localVariant.toInt());
        break;

        // TODO: Add int etc.

    default:
        qWarning("bsonFromVariant only accepts variants with list or map contents");
        break;
    }

}

CLIENT_API void bsonFromVariant(bson *b, const QVariantMap &variant)
{
    bsonAppend(b, variant, QString());
}

namespace
{
    enum ObjectType
    {
        OBJECT,
        LIST,
    };
}
Q_DECLARE_TYPEINFO(ObjectType, Q_PRIMITIVE_TYPE);

QVariantMap variantFromBson(const bson *b)
{
    bson_iterator iter;
    bson_iterator_init(&iter, b);

    QStack<bson_iterator> iteratorStack;
    iteratorStack.push(iter);

    QStack<ObjectType> objectTypes;
    QStack<QVariantMap> maps;
    QStack<QVariantList> lists;

    QByteArray id;

    while(!iteratorStack.isEmpty())
    {
        bson_iterator *iter = &iteratorStack.top();
        ObjectType type = objectTypes.isEmpty() ? OBJECT : objectTypes.top();

        bson_type iteratorType = bson_iterator_next(iter);

        switch(iteratorType)
        {
        case BSON_OBJECT:
        case BSON_ARRAY:
            {
                objectTypes.push(iteratorType == BSON_OBJECT ? OBJECT : LIST);

                if (iteratorType == BSON_OBJECT)
                {
                    maps.push(QVariantMap());
                }
                else
                {
                    lists.push(QVariantList());
                }

                bson_iterator subiterator;
                bson_iterator_subiterator(iter, &subiterator);

                iteratorStack.push(subiterator);
            }
            break;

        case BSON_EOO: // End of object
            {
                QVariant value;
                if (type == OBJECT)
                {
                    // Return last object
                    if (maps.size() == 1)
                    {
                        QVariantMap &map = maps.top();
                        map.insert("id", id);
                        return map;
                    }

                    value = maps.pop();
                }
                else
                {
                    value = lists.pop();
                }

                objectTypes.pop();
                iteratorStack.pop();

                switch(objectTypes.top())
                {
                case OBJECT:
                    {
                        QString key = QString::fromLatin1(bson_iterator_key(&iteratorStack.top()));
                        maps.top().insert(key, value);
                    }
                    break;

                case LIST:
                    lists.top().append(value);
                    break;
                }
            }
            break;

        case BSON_DOUBLE:
            break;

        case BSON_STRING:
            {
                QString value = QString::fromLatin1(bson_iterator_string(iter));

                if (type == OBJECT)
                {
                    QString key = QString::fromLatin1(bson_iterator_key(iter));
                    maps.top().insert(key, value);
                }
                else
                {
                    lists.top().append(value);
                }
            }
            break;

        case BSON_BOOL:
            break;

        case BSON_OID:
        {
            bson_oid_t *oid = bson_iterator_oid(iter);

            id.resize(25);
            bson_oid_to_string(oid, id.data());
        }
        break;

        case BSON_INT:
        {
            int value = bson_iterator_int(iter);

            if (type == OBJECT)
            {
                QString key = QString::fromLatin1(bson_iterator_key(iter));
                maps.top().insert(key, value);
            }
            else
            {
                lists.top().append(value);
            }
        }
        break;

        default:
            qDebug() << "Unknown iterator type:" << iteratorType;
            break;
        }
    }

    qWarning("End of variantFromBson reached, this should never happen");
    return QVariantMap();
}
