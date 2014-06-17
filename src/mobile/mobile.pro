TEMPLATE = app

# Add more folders to ship with the application, here
folder_01.source = qml
folder_01.target = ""
DEPLOYMENTFOLDERS = folder_01

qmlfiles.files = $$PWD/qml $$PWD/Default-568h@2x.png
qmlfiles.path = Contents/Resources
#QMAKE_BUNDLE_DATA += qmlfiles

ios {
    qmlimports.files = $$[QT_INSTALL_QML]
    qmlimports.path = imports
    QMAKE_BUNDLE_DATA += qmlimports
}

QT += gui quick qml concurrent

include($$SOURCE_DIRECTORY/dist.pri)
include($$SOURCE_DIRECTORY/src/libclient/libclient.pri)

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android
ANDROID_PACKAGE = "net.measureit.glimpse"
ANDROID_APP_NAME = "glimpse"

android {
    QT += androidextras

    HEADERS += statusbarhelper.h
    SOURCES += statusbarhelper.cpp

    OTHER_FILES += $$files($$PWD/android/src/*.java) \
                   android/AndroidManifest.xml

    system(sh $$PWD/qmlresources.sh)

    RESOURCES += qmlresources.qrc
} else: ios {
    LIBS += $$[QT_INSTALL_QML]/QtQuick/Controls/libqtquickcontrolsplugin.a \
            $$[QT_INSTALL_QML]/QtQuick/Window.2/libwindowplugin.a \
            $$[QT_INSTALL_QML]/QtQuick.2/libqtquick2plugin.a

    LIBS += -framework AdSupport
} else {
    QT += widgets

    HEADERS += desktopstatusbarhelper.h
    SOURCES += desktopstatusbarhelper.cpp
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

# Please do not modify the following two lines. Required for deployment.
include(qtquick2applicationviewer/qtquick2applicationviewer.pri)
qtcAddDeployment()

OTHER_FILES += $$files(qml/*, true)

