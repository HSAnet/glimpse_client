#include "androidprocessmodel.h"
#include "androidhelper.h"

#include <QList>

struct ProcessInfo
{
    QString packageName;
    QString displayName;
};

class AndroidProcessModelPrivate
{
public:
    AndroidProcessModelPrivate(AndroidProcessModel* q)
    : q(q)
    {
        Java env;
        processHelperClass = env.findClass("de/hsaugsburg/informatik/mplane/ProcessHelper");
        runningProcesses = env->GetMethodID(processHelperClass, "runningProcesses", "()Ljava/util/List;");

        jclass processInfoClass = env.findClass("de/hsaugsburg/informatik/mplane/ProcessHelper$ProcessInfo");
        displayName = env->GetFieldID(processInfoClass, "displayName", "Ljava/lang/String;");
        packageName = env->GetFieldID(processInfoClass, "packageName", "Ljava/lang/String;");

        jclass listClass = env.findClass("java/util/List");
        listSize = env->GetMethodID(listClass, "size", "()I");
        listGet = env->GetMethodID(listClass, "get", "(I)Ljava/lang/Object;");
    }

    AndroidProcessModel* q;

    // Properties
    QList<ProcessInfo> processes;

    jclass processHelperClass;
    jmethodID runningProcesses;
    jmethodID listSize;
    jmethodID listGet;
    jfieldID displayName;
    jfieldID packageName;
};

AndroidProcessModel::AndroidProcessModel(QObject *parent)
: QAbstractListModel(parent)
, d(new AndroidProcessModelPrivate(this))
{
    reload();
}

AndroidProcessModel::~AndroidProcessModel()
{
    delete d;
}

int AndroidProcessModel::rowCount(const QModelIndex &parent) const
{
    if ( parent.isValid() )
        return 0;

    return d->processes.size();
}

QVariant AndroidProcessModel::data(const QModelIndex &index, int role) const
{
    int row = index.row();
    if (row < 0 || row >= d->processes.size())
        return QVariant();

    const ProcessInfo& info = d->processes.at(row);

    switch(role) {
    case PackageNameRole:
        return info.packageName;

    case DisplayNameRole:
        return info.displayName;

    default:
        break;
    }

    return QVariant();
}

QHash<int, QByteArray> AndroidProcessModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles.insert(PackageNameRole, "packageName");
    roles.insert(DisplayNameRole, "displayName");
    return roles;
}

void AndroidProcessModel::reload()
{
    beginResetModel();
    d->processes.clear();

    Java env;
    jobject processHelper = env.createInstance(d->processHelperClass);
    jobject processList = env->CallObjectMethod(processHelper, d->runningProcesses);

    jint size = env->CallIntMethod(processList, d->listSize);
    for(int i=0; i < size; ++i) {
        jobject jprocess = env->CallObjectMethod(processList, d->listGet, i);
        jstring jpackageName = (jstring)env->GetObjectField(jprocess, d->packageName);
        jstring jdisplayName = (jstring)env->GetObjectField(jprocess, d->displayName);

        ProcessInfo info;
        info.packageName = getQString(env, jpackageName);
        info.displayName = getQString(env, jdisplayName);

        d->processes.append(info);
    }

    env->DeleteGlobalRef(processHelper);

    endResetModel();
}

namespace {
    static int init_AndroidProcessModel() {
        Java::registerClass("de/hsaugsburg/informatik/mplane/ProcessHelper");
        Java::registerClass("de/hsaugsburg/informatik/mplane/ProcessHelper$ProcessInfo");
        Java::registerClass("java/util/List");
        return 1;
    }

    static int __AndroidProcessModel = init_AndroidProcessModel();
}
