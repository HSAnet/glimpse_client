#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "../export.h"

#include <QObject>

class CLIENT_API Controller : public QObject
{
    Q_OBJECT
    Q_ENUMS(Status)
    Q_PROPERTY(Status status READ status NOTIFY statusChanged)

public:
    explicit Controller(QObject* parent = 0);
    ~Controller();

    enum Status {
        Unknown,
        Running,
        Finished,
        Error
    };

    virtual Status status() const = 0;
    Q_INVOKABLE virtual QString errorString() const = 0;

signals:
    void statusChanged();

    void started();
    void error();
    void finished();
};

#endif // CONTROLLER_H
