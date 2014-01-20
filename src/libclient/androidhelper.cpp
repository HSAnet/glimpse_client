#include "androidhelper.h"
#include "log/logger.h"

#include <QList>
#include <QHash>
#include <QDebug>
#include <QCoreApplication>

LOGGER(Java);

namespace
{
static JavaVM* g_vm = NULL;

QList<const char*>& registeredClasses()
{
    static QList<const char*> classes;
    return classes;
}

static QHash<QByteArray, jclass> classMap;
}

QString getQString(const Java& env, jstring str)
{
    const jchar* chars = env->GetStringChars(str, NULL);
    QString msg = QString::fromUtf16(chars, env->GetStringLength(str));
    env->ReleaseStringChars(str, chars);
    return msg;
}

static void logInfo(JNIEnv* env, const QString& tag, const QString& message)
{
    jclass c = env->FindClass("android/util/Log");
    if (c)
    {
        jmethodID m = env->GetStaticMethodID(c, "i", "(Ljava/lang/String;Ljava/lang/String;)I");

        jstring jTag = env->NewString((const jchar*)tag.constData(), tag.length());
        jstring jMessage = env->NewString((const jchar*)message.constData(), message.length());
        env->CallStaticIntMethod(c, m, tag.constData(), jTag, jMessage);
        env->DeleteLocalRef(jMessage);
        env->DeleteLocalRef(jTag);

        env->DeleteLocalRef(c);

        if (env->ExceptionCheck())
        {
            printf("Exception!!\n");
            env->ExceptionClear();
        }
    }
}

static void dispatchOnDestroy(JNIEnv *env, jclass clazz)
{
    Q_UNUSED(clazz);

    //logInfo(env, "mPlane", "Quitting Application");

    //qApp->quit();
}

static JNINativeMethod method_table[] =
{
    { "dispatchOnDestroy", "()V", (void*)dispatchOnDestroy }
};

static int method_table_size = sizeof(method_table) / sizeof(method_table[0]);

JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void*)
{
    g_vm = vm;

    JNIEnv* env;
    vm->GetEnv((void**)&env, JNI_VERSION_1_6);

    jclass clazz = env->FindClass("de/hsaugsburg/informatik/mplane/MobileActivity");
    if (clazz)
    {
        //env->RegisterNatives(clazz, method_table, method_table_size);
        env->DeleteLocalRef(clazz);
    }
    else
    {
        return JNI_ERR;
    }

    foreach(const char* className, registeredClasses())
    {
        clazz = env->FindClass(className);
        if ( clazz )
        {
            classMap.insert(className, (jclass)env->NewGlobalRef(clazz));
        }
        else
        {
            LOG_ERROR(QString("Failed to find class %1").arg(className));
        }
    }

    return JNI_VERSION_1_6;
}

JNIEXPORT void JNI_OnUnload(JavaVM* vm, void*)
{
    JNIEnv* env;
    vm->GetEnv((void**)&env, JNI_VERSION_1_6);

    foreach(jclass clazz, classMap)
    {
        env->DeleteGlobalRef(clazz);
    }
}

JavaVM* javaVM()
{
    return g_vm;
}

bool Java::attach()
{
    return (valid = javaVM()->AttachCurrentThread(&env, 0)) >= 0;
}

void Java::detach()
{
    javaVM()->DetachCurrentThread();
}

bool Java::isValid() const
{
    return valid;
}

JNIEnv *Java::operator ->() const
{
    return env;
}

void Java::registerClass(const char *className)
{
    if (!registeredClasses().contains(className))
    {
        registeredClasses().append(className);
    }
}

jclass Java::findClass(const QByteArray &className) const
{
    return classMap.value(className);
}

jobject Java::createInstance(jclass clazz) const
{
    if ( clazz == NULL )
    {
        return NULL;
    }

    jmethodID constructor = env->GetMethodID(clazz, "<init>", "()V");
    if ( !constructor )
    {
        jmethodID nameId = env->GetMethodID(clazz, "getName", "()Ljava/lang/String;");
        QString className = getQString(*this, (jstring)env->CallObjectMethod(clazz, nameId));
        LOG_WARNING(QString("No default contructor found for class %1").arg(className));
        return NULL;
    }

    return env->NewGlobalRef( env->NewObject(clazz, constructor) );
}

jobject Java::createInstance(const QByteArray &className) const
{
    return createInstance(findClass(className));
}
