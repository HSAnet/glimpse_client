#include "testfactory.h"

// TODO: "Pluginize" tests
#include "tests/packettrain.h"
#include "tests/speedtest.h"

TestFactory::TestFactory()
{
}

TestFactory::~TestFactory()
{
}

QStringList TestFactory::availableTests() const
{
    QStringList availableTests;
    availableTests.append("packettrain");
    availableTests.append("speedtest");
    return availableTests;
}

AbstractTest *TestFactory::createTest(const QString &name) const
{
    if (name == "packettrain") {
        return new PacketTrain;
    } else if (name == "speedtest")
        return new SpeedTest;

    return NULL;
}
