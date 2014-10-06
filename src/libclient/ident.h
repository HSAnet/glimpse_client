#ifndef IDENT_H
#define IDENT_H

#include <QMetaType>
#include <QHash>

/**
 * Identifier base class
 */
template <typename T>
class Ident
{
public:
    Ident()
    : m_id(-1)
    {
    }

    explicit Ident(int id)
    : m_id(id)
    {
    }

    bool operator==(const Ident<T> &rhs) const
    {
        return rhs.m_id == m_id;
    }

    int toInt() const
    {
        return m_id;
    }

    bool isValid() const
    {
        return m_id != -1;
    }

private:
    int m_id;
};

template <>
class Ident<class Report>
{
public:
    Ident()
    {
    }

    explicit Ident(const QByteArray &id)
    : m_id(id)
    {
    }

    bool operator==(const Ident<class Report> &rhs) const
    {
        return rhs.m_id == m_id;
    }

    QByteArray toByteArray() const
    {
        return m_id;
    }

    bool isValid() const
    {
        return !m_id.isNull();
    }

private:
    QByteArray m_id;
};

template <typename T>
inline uint qHash(const Ident<T> &id)
{
    return qHash(id.toInt());
}

template <>
inline uint qHash(const Ident<class Report> &id)
{
    return qHash(id.toByteArray());
}

// Type definitions
typedef Ident<class TestDefinition> TaskId;
typedef Ident<class Result> ResultId;
typedef Ident<class Report> ReportId;

// Metatype declarations
Q_DECLARE_METATYPE(TaskId)
Q_DECLARE_METATYPE(ResultId)
Q_DECLARE_METATYPE(ReportId)

#endif // IDENT_H
