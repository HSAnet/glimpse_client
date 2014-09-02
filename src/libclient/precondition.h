#ifndef PRECONDITION_H
#define PRECONDITION_H

#include <QList>
#include <QVariant>

class Precondition;
typedef QList<Precondition> PreconditionList;

class Precondition
{
public:
    Precondition();
    Precondition(bool onWireless, bool onWire, bool onCellular, quint16 minCharge, double locLat, double locLong, double locRadius);

    static Precondition fromVariant(const QVariant &variant);
    QVariant toVariant() const;

    bool check();

protected:
    class Private;
    Private *d;
};

#endif // PRECONDITION_H
