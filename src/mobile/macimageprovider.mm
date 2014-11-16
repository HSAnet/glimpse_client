#include "macimageprovider.h"

#include <QtMac>

#import <AppKit/NSRunningApplication.h>
#import <AppKit/NSImage.h>

namespace QtMac {
    NSString* toNSString();
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
    NSString* bundleIdentifier = id.toNSString();
    NSArray* apps = [NSRunningApplication runningApplicationsWithBundleIdentifier:bundleIdentifier];

    if ([apps count] == 0)
        return QImage();

    NSRunningApplication* app = (NSRunningApplication*)[apps firstObject];
    NSImage* icon = [app icon];
    CGImageRef image = [icon CGImageForProposedRect:NULL context:nil hints:nil];

    return QtMac::fromCGImageRef(image).toImage();
}
