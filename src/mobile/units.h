#ifndef UNITS_H
#define UNITS_H

#include "export.h"

#include <QObject>

class Units : public QObject
{
    Q_OBJECT
    Q_PROPERTY(float gridUnit READ gridUnit WRITE setGridUnit NOTIFY gridUnitChanged)

public:
    explicit Units(QObject *parent = 0);
    ~Units();

    Q_INVOKABLE float gu(float value) const;
    Q_INVOKABLE float dp(float value) const;

    void setGridUnit(float gridUnit);
    float gridUnit() const;

signals:
    void gridUnitChanged();

protected:
    class Private;
    Private *d;
};

#endif // UNITS_H
