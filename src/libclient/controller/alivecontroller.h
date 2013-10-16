#ifndef ALIVECONTROLLER_H
#define ALIVECONTROLLER_H

#include "../export.h"

#include <QObject>

class QAbstractSocket;
class Settings;

class CLIENT_API AliveController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool running READ isRunning WRITE setRunning NOTIFY runningChanged)

public:
    AliveController(QObject* parent = 0);
    ~AliveController();

    bool init(Settings* settings);

    void setSocket(QAbstractSocket* socket);
    QAbstractSocket *socket() const;

    void setRunning(bool running);
    bool isRunning() const;

signals:
    void runningChanged();

protected:
    class Private;
    Private* d;
};

#endif // ALIVECONTROLLER_H
