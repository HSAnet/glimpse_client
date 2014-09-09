TEMPLATE = app

qmlfiles.files = $$PWD/Default-568h@2x.png
qmlfiles.path = Contents/Resources
QMAKE_BUNDLE_DATA += qmlfiles

QT += gui quick qml concurrent

include(../../dist.pri)
include(../libclient/libclient.pri)

osx:TARGET = glimpse
osx:ICON = glimpse.icns
osx:QMAKE_INFO_PLIST = Info.plist

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android
ANDROID_PACKAGE = "net.measureit.glimpse"
ANDROID_APP_NAME = "glimpse"

# Only deploy qt files in resources
system(sh $$PWD/qmlresources.sh)
RESOURCES += qmlresources.qrc

android {
    QT += androidextras

    HEADERS += statusbarhelper.h
    SOURCES += statusbarhelper.cpp

    OTHER_FILES += $$files($$PWD/android/src/*.java) \
                   android/AndroidManifest.xml
} else: ios {
    LIBS += -framework AdSupport
} else {
    QT += widgets

    HEADERS += desktopstatusbarhelper.h
    SOURCES += desktopstatusbarhelper.cpp
}

win32 {
    conf.files = qt.conf
    conf.path = $${target.path}
    INSTALLS += conf
}

osx {
    QT += macextras
    LIBS += -framework AppKit -framework CoreFoundation -framework DiskArbitration
}

!android:linux {
    CONFIG += link_pkgconfig
    PKGCONFIG += libwnck-1.0
    DEFINES += HAVE_WNCK
}

SOURCES += \
    main.cpp

RESOURCES += \
    resources/resource.qrc

# Dump symbols
#symdump.commands = dump_syms $$qtLibraryTarget($$TARGET) 2>/dev/null > "$${TARGET}.sym"
#QMAKE_EXTRA_TARGETS += symdump
#POST_TARGETDEPS += symdump

OTHER_FILES += $$files(qml/*) \
               $$files(qml/controls/*) \
               $$files(qml/android/*) \
               qt.conf \
               Info.plist

