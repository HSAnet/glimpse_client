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

template <typename T>
inline uint qHash(const Ident<T> &id)
{
    return qHash(id.toInt());
}

// Type definitions
typedef Ident<class TestDefinition> ScheduleId;
typedef Ident<class Result> ResultId;
typedef Ident<class Report> ReportId;
typedef Ident<class Measurement> TaskId;


// Metatype declarations
Q_DECLARE_METATYPE(ScheduleId)
Q_DECLARE_METATYPE(ResultId)
Q_DECLARE_METATYPE(ReportId)
Q_DECLARE_METATYPE(TaskId)

#endif // IDENT_H
