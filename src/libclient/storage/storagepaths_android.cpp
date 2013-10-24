#include "storagepaths.h"
#include "../log/logger.h"
#include "../androidhelper.h"

LOGGER(StoragePaths);

class StoragePaths::Private
{
public:
    static bool initialized;
    static QDir scheduler;
    static QDir report;
    static QDir cache;
};

bool StoragePaths::Private::initialized = false;
QDir StoragePaths::Private::scheduler;
QDir StoragePaths::Private::report;
QDir StoragePaths::Private::cache;

StoragePaths::StoragePaths()
{
    if (StoragePaths::Private::initialized)
        return;

    Java env;
    jclass clazz = env.findClass("de/hsaugsburg/informatik/mplane/StorageHelper");
    jobject instance = env.createInstance(clazz);

    jmethodID schedulerId = env->GetMethodID(clazz, "getSchedulerDirectory", "()Ljava/lang/String;");
    jmethodID reportId = env->GetMethodID(clazz, "getReportDirectory", "()Ljava/lang/String;");
    jmethodID cacheId = env->GetMethodID(clazz, "getCacheDirectory", "()Ljava/lang/String;");

    jstring schedulerPath = (jstring)env->CallObjectMethod(instance, schedulerId);
    jstring reportPath = (jstring)env->CallObjectMethod(instance, reportId);
    jstring cachePath = (jstring)env->CallObjectMethod(instance, cacheId);

    StoragePaths::Private::scheduler = getQString(env, schedulerPath);
    StoragePaths::Private::report = getQString(env, reportPath);
    StoragePaths::Private::cache = getQString(env, cachePath);
    StoragePaths::Private::initialized = true;

    env->DeleteGlobalRef(instance);
}

StoragePaths::~StoragePaths()
{
}

QDir StoragePaths::schedulerDirectory() const
{
    return StoragePaths::Private::scheduler;
}

QDir StoragePaths::reportDirectory() const
{
    return StoragePaths::Private::report;
}

QDir StoragePaths::cacheDirectory() const
{
    return StoragePaths::Private::cache;
}

namespace {
    static int init_StoragePaths() {
        Java::registerClass("de/hsaugsburg/informatik/mplane/StorageHelper");
        return 1;
    }

    static int __StoragePaths = init_StoragePaths();
}
