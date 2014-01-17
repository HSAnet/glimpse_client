#ifndef ANDROIDHELPER_H
#define ANDROIDHELPER_H

#include "export.h"

#include <jni.h>
#include <QByteArray>

JavaVM* CLIENT_API javaVM();

class CLIENT_API Java
{
public:
    Java()
    : valid(false)
    , env(NULL)
    {
        attach();
    }

    ~Java() {
        detach();
    }

    JNIEnv* operator ->() const;

    bool attach();
    void detach();

    bool isValid() const;

    static void registerClass(const char* className);
    jclass findClass(const QByteArray& className) const;
    jobject createInstance(jclass clazz) const;
    jobject createInstance(const QByteArray& className) const;

protected:
    bool valid;
    JNIEnv* env;
};

QString CLIENT_API getQString(const Java &env, jstring str);

#endif // ANDROIDHELPER_H
