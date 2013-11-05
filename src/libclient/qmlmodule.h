#ifndef QMLMODULE_H
#define QMLMODULE_H

#include "export.h"

class QQmlEngine;

class CLIENT_API QmlModule
{
public:
    static void registerTypes();
    static void initializeEngine(QQmlEngine* engine);
};

#endif // QMLMODULE_H
