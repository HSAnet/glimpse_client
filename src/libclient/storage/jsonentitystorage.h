#ifndef JSONENTITYSTORAGE_H
#define JSONENTITYSTORAGE_H

#include "../export.h"
#include "entitystorage.h"

#include <QVariant>
#include <QJsonDocument>
#include <QJsonObject>
#include <tcejdb/ejdb.h>

CLIENT_API void bsonFromVariant(bson *b, const QVariantMap &variant);
CLIENT_API QVariantMap variantFromBson(const bson *b);

template <typename EntityType, typename IdType>
class JSONEntityStorage : public EntityStorage<EntityType, IdType>
{
public:
    JSONEntityStorage()
    : jb(NULL)
    , coll(NULL)
    {
    }

    ~JSONEntityStorage()
    {
        ejdbclose(jb);
        ejdbdel(jb);
    }

    QList<IdType> add(const QList<EntityType> &entities)
    {
        QList<IdType> ids;

        foreach(const EntityType &entity, entities)
        {
            bson b;
            bson_init(&b);
            bsonFromVariant(&b, entity.toVariant().toMap());
            bson_finish(&b);

            bson_oid_t id;
            ejdbsavebson(coll, &b, &id);

            QByteArray idString;
            idString.resize(25);
            bson_oid_to_string(&id, idString.data());

            ids << IdType(idString);

            bson_destroy(&b);
        }

        return ids;
    }

    bool remove(const QList<IdType> &entityIds)
    {
        foreach(const IdType &id, entityIds)
        {
            bson_oid_t oid;
            bson_oid_from_string(&oid, id.toByteArray());

            ejdbrmbson(coll, &oid);
        }

        return true;
    }

    void update(const QList<EntityType> &entities)
    {
        foreach(const EntityType &entity, entities)
        {
            bson b;
            bson_init(&b);
            bsonFromVariant(&b, entity.toVariant().toMap());
            bson_finish(&b);

            bson_oid_t id;
            bson_oid_from_string(&id, entity.id().toByteArray());

            ejdbsavebson2(coll, &b, &id, false);

            bson_destroy(&b);
        }
    }

    QList<EntityType> allEntities()
    {
        // Create an empty query object
        bson b;
        bson_init_as_query(&b);
        bson_finish(&b);

        EJQ *q = ejdbcreatequery(jb, &b, NULL, 0, NULL);

        uint32_t count;
        TCLIST *result = ejdbqryexecute(coll, q, &count, 0, NULL);

        QList<EntityType> entities;

        for (uint32_t i=0; i < count; ++i)
        {
            const void *bsdata = (const void *)TCLISTVALPTR(result, i);

            bson b;
            bson_init_with_data(&b, bsdata);
            bson_finish(&b);

            QVariant variant = variantFromBson(&b);

            entities << EntityType::fromVariant(variant);
        }

        tclistdel(result);
        ejdbquerydel(q);

        return entities;
    }

    QList<EntityType> entitiesById(const QList<IdType> &entityIds)
    {
        QList<EntityType> entities;
        return entities;
    }

    bool init(const QString &tableName, const QString &dbName = "glimpse")
    {
        jb = ejdbnew();

        if (!ejdbopen(jb, dbName.toLatin1(), JBOWRITER | JBOCREAT))
        {
            EntityStorage<EntityType, IdType>::m_errorString = "Error opening database";
            return false;
        }

        coll = ejdbcreatecoll(jb, tableName.toLatin1(), NULL);

        return true;
    }

protected:
    EJDB *jb;
    EJCOLL *coll;
};

#endif // JSONENTITYSTORAGE_H
