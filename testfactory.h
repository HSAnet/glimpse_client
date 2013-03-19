#ifndef TESTFACTORY_H
#define TESTFACTORY_H

#include "types.h"

#include <QObject>

class QAbstractSocket;

////////////////////////////////////////////////////////////

class ProtocolNeutralTest
{
public:
    virtual void init(QAbstractSocket* socket) = 0;
};

class UdpProtocolTest : public ProtocolNeutralTest
{
};

class UtpProtocolTest : public ProtocolNeutralTest
{
};

class TcpProtocolTest : public ProtocolNeutralTest
{
};

////////////////////////////////////////////////////////////

struct TestInfo
{
    QString name;
    QString author;
    QString version;
};

////////////////////////////////////////////////////////////

class TestFactory
{
public:
    virtual ~TestFactory() {}

    virtual TestInfo info() const = 0;
    virtual QObject* createTest(Direction direction) = 0;
};

#endif // TESTFACTORY_H
