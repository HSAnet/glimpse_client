#include "macprocessmodel.h"

#include <QStringList>
#include <QMacFunctions>

#import <AppKit/NSWorkspace.h>
#import <AppKit/NSRunningApplication.h>

namespace QtMac {
    QString fromNSString(const NSString *string);
}

class MacProcessModel::Private
{
public:
    QStringList apps;
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
    switch(role) {
    case DisplayNameRole:
        return d->apps.at(index.row());

    default:
        break;
    }

    return QVariant();
}

QHash<int, QByteArray> MacProcessModel::roleNames() const
{
    QHash<int, QByteArray> roleNames;
    roleNames.insert(DisplayNameRole, "displayName");
    return roleNames;
}

void MacProcessModel::reload()
{
    beginResetModel();
    d->apps.clear();

    NSWorkspace* workspace = [NSWorkspace sharedWorkspace];
    NSArray* runningApplications = [workspace runningApplications];

    for(NSRunningApplication* application in runningApplications) {
        NSString* name = [application localizedName];

        d->apps.append(QtMac::fromNSString(name));
    }

    endResetModel();
}
