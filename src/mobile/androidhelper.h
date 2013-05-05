#ifndef ANDROIDHELPER_H
#define ANDROIDHELPER_H

#include <jni.h>
#include <QByteArray>

JavaVM* javaVM();

extern jclass StatusBarHelperClass;

class Java
{
public:
    Java() {
        attach();
    }

    ~Java() {
        detach();
    }

    JNIEnv* operator ->() const {
        return env;
    }

    bool attach() {
        return (valid = javaVM()->AttachCurrentThread(&env, 0)) >= 0;
    }

    void detach() {
        javaVM()->DetachCurrentThread();
    }

    bool isValid() const {
        return valid;
    }

    static void registerClass(const char* className);
    jclass findClass(const QByteArray& className) const;
    jobject createInstance(jclass clazz) const;
    jobject createInstance(const QByteArray& className) const;

protected:
    bool valid;
    JNIEnv* env;
};

#endif // ANDROIDHELPER_H
