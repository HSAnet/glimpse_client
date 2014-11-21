#ifndef QMLMODULE_H
#define QMLMODULE_H

#include "export.h"

class QQmlEngine;

class QmlModule
{
public:
    static void registerTypes();
    static void initializeEngine(QQmlEngine *engine);
};

#endif // QMLMODULE_H
