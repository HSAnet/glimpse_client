#ifndef RESULT_H
#define RESULT_H

#include "../serializable.h"

#include <QDateTime>
#include <QMetaType>
#include <QUuid>

#include <QSharedDataPointer>

class Result;
typedef QList<Result> ResultList;


class ResultData : public QSharedData
{
public:
    ResultData()
    {
    }

    ResultData(const ResultData &other)
    : QSharedData(other)
    , startDateTime(other.startDateTime)
    , endDateTime(other.endDateTime)
    , conflictingTasks()
    , crossTraffic()
    , probeResult(other.probeResult)
    , measureUuid(other.measureUuid)
    , errorString(other.errorString)
    {
    }

    QDateTime startDateTime;
    QDateTime endDateTime;
    QVariant conflictingTasks;
    QVariant crossTraffic;
    QVariantList probeResult;
    QUuid measureUuid;
    QString errorString;
};


class CLIENT_API Result : public Serializable
{
public:
    Result();
    Result(const QString &errorString);
    Result(const Result &other);
    Result(const QDateTime &startDateTime,
           const QDateTime &endDateTime,
           const QVariantList &probeResult,
           const QUuid &measureUuid = QUuid(),
           const QString &errorString = QString());

    bool isNull() const;

    void setStartDateTime(const QDateTime& startDateTime);
    QDateTime startDateTime() const;

    void setEndDateTime(const QDateTime& endDateTime);
    QDateTime endDateTime() const;

    void setProbeResult(const QVariantList& probeResult);
    QVariantList probeResult() const;

    void setMeasureUuid(const QUuid& measureUuid);
    QUuid measureUuid() const;

    void setErrorString(const QString &errorString);
    QString errorString() const;

    // Storage
    static Result fromVariant(const QVariant &variant);

    // Serializable interface
    QVariant toVariant() const;

private:
    QSharedDataPointer<ResultData> d;
};


inline QVariant qVariantFromValue(const Result &result)
{
    return result.toVariant();
}


Q_DECLARE_METATYPE(Result)
Q_DECLARE_TYPEINFO(Result, Q_MOVABLE_TYPE);
Q_DECLARE_METATYPE(ResultList)

#endif // RESULT_H
