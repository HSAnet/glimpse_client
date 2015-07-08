#ifndef RESULT_H
#define RESULT_H

#include "../ident.h"
#include "../serializable.h"

#include <QDateTime>
#include <QMetaType>
#include <QUuid>

#include <QSharedDataPointer>

class Result;
typedef QList<Result> ResultList;

class CLIENT_API Result : public Serializable
{
public:
    Result();
    Result(const QString &errorString);
    Result(const Result &other);
    Result(const QVariantList &probeResult,
           const QUuid &measureUuid = QUuid());
    Result(const QDateTime &startDateTime,
           const QDateTime &endDateTime,
           qint64 duration,
           const QVariantList &probeResult,
           const QUuid &measureUuid,
           const QVariantMap &preInfo,
           const QVariantMap &postInfo,
           const QString &errorString);
    ~Result();

    Result &operator=(const Result &rhs);

    bool isNull() const;

    void setStartDateTime(const QDateTime& startDateTime);
    QDateTime startDateTime() const;

    void setEndDateTime(const QDateTime& endDateTime);
    QDateTime endDateTime() const;

    void setProbeResult(const QVariantList &probeResult);
    QVariantList probeResult() const;

    void setPreInfo(const QVariantMap& preInfo);
    QVariantMap preInfo() const;

    void setPostInfo(const QVariantMap& postInfo);
    QVariantMap postInfo() const;

    void setMeasureUuid(const QUuid& measureUuid);
    QUuid measureUuid() const;

    void setErrorString(const QString &errorString);
    QString errorString() const;

    // Storage
    static Result fromVariant(const QVariant &variant);

    // Serializable interface
    QVariant toVariant() const;
    QVariant toVariantStripped() const;

private:
    QSharedDataPointer<class ResultData> d;
};


inline QVariant qVariantFromValue(const Result &result)
{
    return result.toVariant();
}


Q_DECLARE_METATYPE(Result)
Q_DECLARE_TYPEINFO(Result, Q_MOVABLE_TYPE);
Q_DECLARE_METATYPE(ResultList)

#endif // RESULT_H
