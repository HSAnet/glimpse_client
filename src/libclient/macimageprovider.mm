#include "macimageprovider.h"

#include <QtMacExtras>

#import <AppKit/NSRunningApplication.h>
#import <AppKit/NSImage.h>

namespace QtMac {
    NSString* toNSString(const QString& string);
}

MacImageProvider::MacImageProvider()
: QQuickImageProvider(Image)
{
}

MacImageProvider::~MacImageProvider()
{
}

QImage MacImageProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize)
{
    NSString* bundleIdentifier = QtMac::toNSString(id);
    NSArray* apps = [NSRunningApplication runningApplicationsWithBundleIdentifier:bundleIdentifier];

    if ([apps count] == 0)
        return QImage();

    NSRunningApplication* app = (NSRunningApplication*)[apps firstObject];
    NSImage* icon = [app icon];
    CGImageRef image = [icon CGImageForProposedRect:NULL context:nil hints:nil];

    return QtMac::fromCGImageRef(image).toImage();
}
