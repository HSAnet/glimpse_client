#ifndef RESULT_H
#define RESULT_H

#include "../serializable.h"

#include <QDateTime>
#include <QMetaType>

class Result;
typedef QSharedPointer<Result> ResultPtr;
typedef QList<ResultPtr> ResultList;

class CLIENT_API Result : public Serializable
{
public:
    Result(const QDateTime& dateTime,
           const QVariant& probeResult,
           const QVariant& peerResult);
    ~Result();

    // Storage
    static ResultPtr fromVariant(const QVariant& variant);

    // Getters
    QDateTime dateTime() const;
    QVariant probeResult() const;
    QVariant peerResult() const;

    // Serializable interface
    QVariant toVariant() const;

protected:
    class Private;
    Private* d;
};

Q_DECLARE_METATYPE(ResultPtr)
Q_DECLARE_METATYPE(ResultList)

#endif // RESULT_H
