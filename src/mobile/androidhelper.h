#ifndef ANDROIDHELPER_H
#define ANDROIDHELPER_H

#include <jni.h>
#include <QByteArray>

JavaVM* javaVM();

class Java
{
public:
    Java() {
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

#endif // ANDROIDHELPER_H
