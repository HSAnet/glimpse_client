#include "floodtest.h"

FloodTest::FloodTest(Direction direction, QObject *parent)
: QObject(parent)
, direction(direction)
{
}

void FloodTest::init(QAbstractSocket *socket)
{
    this->socket = socket;
}

////////////////////////////////////////////////////////////

TestInfo FloodTestFactory::info() const
{
    TestInfo info;
    info.name = "Flood";
    info.author = "asdf";
    info.version = "0.1";
    return info;
}

QObject *FloodTestFactory::createTest(Direction direction)
{
    return new FloodTest(direction);
}
