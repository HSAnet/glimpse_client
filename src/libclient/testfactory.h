#ifndef TESTFACTORY_H
#define TESTFACTORY_H

#include "tests/test.h"

#include <QString>

class CLIENT_API TestFactory
{
public:
    TestFactory();
    ~TestFactory();

    QStringList availableTests() const;

    AbstractTest* createTest(const QString& name) const;
};

#endif // TESTFACTORY_H
