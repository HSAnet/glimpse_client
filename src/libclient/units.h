#ifndef UNITS_H
#define UNITS_H

#include "export.h"

#include <QObject>

class CLIENT_API Units : public QObject
{
public:
    explicit Units(QObject* parent = 0);
    ~Units();

    Q_INVOKABLE float gu(float value) const;
    Q_INVOKABLE float dp(float value) const;

protected:
    class Private;
    Private* d;
};

#endif // UNITS_H
