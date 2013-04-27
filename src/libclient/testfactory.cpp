#include "testfactory.h"

// TODO: "Pluginize" tests
#include "tests/packettrain.h"

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
    availableTests.append("downloadfiles");
    return availableTests;
}

AbstractTest *TestFactory::createTest(const QString &name) const
{
    if (name == "packettrain") {
        return new PacketTrain;
    }

    return NULL;
}
