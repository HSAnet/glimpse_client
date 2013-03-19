#ifndef FLOODTEST_H
#define FLOODTEST_H

#include "testfactory.h"

#include <QObject>

class FloodTest : public QObject
                , public TcpProtocolTest
{
    Q_OBJECT

public:
    explicit FloodTest(Direction direction, QObject *parent = 0);

    void init(QAbstractSocket *socket);

protected:
    Direction direction;
    QAbstractSocket* socket;
};

////////////////////////////////////////////////////////////

class FloodTestFactory : public TestFactory
{
public:
    TestInfo info() const;
    QObject* createTest(Direction direction);
};

#endif // FLOODTEST_H
