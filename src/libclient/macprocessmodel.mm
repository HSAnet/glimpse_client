#include "macprocessmodel.h"

#include <QStringList>
#include <QMacFunctions>

#import <AppKit/NSWorkspace.h>
#import <AppKit/NSRunningApplication.h>

namespace QtMac {
    QString fromNSString(const NSString *string);
}

struct ProcessInfo
{
    QString localizedName;
    QString bundleName;
};

class MacProcessModel::Private
{
public:
    QList<ProcessInfo> apps;
};

MacProcessModel::MacProcessModel(QObject *parent)
: QAbstractListModel(parent)
, d(new Private)
{
    reload();
}

MacProcessModel::~MacProcessModel()
{
    delete d;
}

int MacProcessModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return d->apps.size();
}

QVariant MacProcessModel::data(const QModelIndex &index, int role) const
{
    const ProcessInfo& app = d->apps.at(index.row());

    switch(role) {
    case DisplayNameRole: return app.localizedName;
    case BundleName: return app.bundleName;

    default:
        break;
    }

    return QVariant();
}

QHash<int, QByteArray> MacProcessModel::roleNames() const
{
    QHash<int, QByteArray> roleNames;
    roleNames.insert(DisplayNameRole, "displayName");
    roleNames.insert(BundleName, "packageName");
    return roleNames;
}

void MacProcessModel::reload()
{
    beginResetModel();
    d->apps.clear();

    NSWorkspace* workspace = [NSWorkspace sharedWorkspace];
    NSArray* runningApplications = [workspace runningApplications];

    for(NSRunningApplication* application in runningApplications) {
        ProcessInfo info;
        info.localizedName = QtMac::fromNSString([application localizedName]);
        info.bundleName = QtMac::fromNSString([application bundleIdentifier]);

        d->apps.append(info);
    }

    endResetModel();
}
