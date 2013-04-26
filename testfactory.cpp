#include "testfactory.h"

// TODO: "Pluginize" tests
#include "tests/packettrain.h"

TestFactory::TestFactory()
{
}

TestFactory::~TestFactory()
{
}

AbstractTest *TestFactory::createTest(const QString &name) const
{
    if (name == "packettrain") {
        return new PacketTrain;
    }

    return NULL;
}
