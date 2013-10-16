#ifndef LOGINCONTROLLER_H
#define LOGINCONTROLLER_H

#include "../export.h"

#include <QObject>

class CLIENT_API LoginController : public QObject
{
    Q_OBJECT

public:
    LoginController(QObject* parent = 0);
    ~LoginController();

protected:
};

#endif // LOGINCONTROLLER_H
