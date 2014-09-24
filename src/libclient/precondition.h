#ifndef PRECONDITION_H
#define PRECONDITION_H

#include <QList>
#include <QVariant>
#include <QSharedDataPointer>

class PreconditionData;
class Precondition;
typedef QList<Precondition> PreconditionList;

class Precondition
{
public:
    Precondition();
    Precondition(const Precondition &rhs);
    Precondition(bool onWireless, bool onWire, bool onCellular, quint16 minCharge, double locLat, double locLong, double locRadius);
    ~Precondition();

    Precondition &operator=(const Precondition &rhs);

    static Precondition fromVariant(const QVariant &variant);
    QVariant toVariant() const;

    bool check();

private:
    QSharedDataPointer<PreconditionData> d;
};

#endif // PRECONDITION_H
