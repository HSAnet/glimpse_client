#ifndef ENTITYSTORAGE_H
#define ENTITYSTORAGE_H

#include <QString>
#include <QList>

template <typename EntityType, typename IdType>
class EntityStorage
{
public:
    QString errorString() const
    {
        return m_errorString;
    }

    IdType add(const EntityType &entity)
    {
        const QList<IdType> ids = add(QList<EntityType>() << entity);
        return ids.isEmpty() ? IdType() : ids.first();
    }

    bool remove(const IdType &entityId)
    {
        return remove(QList<IdType>() << entityId);
    }

    void update(const EntityType &entity)
    {
        update(QList<EntityType>() << entity);
    }

    EntityType entityById(const IdType &entityId)
    {
        const QList<EntityType> entities = entitiesById(QList<IdType>() << entityId);
        return entities.isEmpty() ? EntityType() : entities.first();
    }

    virtual QList<IdType> add(const QList<EntityType> &entities) = 0;
    virtual bool remove(const QList<IdType> &entityIds) = 0;
    virtual void update(const QList<EntityType> &entities) = 0;

    virtual QList<EntityType> allEntities() = 0;
    virtual QList<EntityType> entitiesById(const QList<IdType> &entityIds) = 0;

protected:
    QString m_errorString;
};

#endif // ENTITYSTORAGE_H
