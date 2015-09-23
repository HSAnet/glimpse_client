TEMPLATE = app
TARGET = glimpse-gui

QT += gui quick qml concurrent

CONFIG += qtquickcompiler

include($$SOURCE_DIRECTORY/dist.pri)
include($$SOURCE_DIRECTORY/3rdparty/qt-google-analytics/qt-google-analytics.pri)
include($$SOURCE_DIRECTORY/src/libclient/libclient.pri)

osx:TARGET = glimpse
osx:ICON = glimpse.icns
osx:QMAKE_INFO_PLIST = Info.plist

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android
ANDROID_PACKAGE = "net.measureit.glimpse"
ANDROID_APP_NAME = "glimpse"

# Fetch files for qmlresources.qrc
INPUT_FILES = $$files(qml/*.*, true)
for(file, INPUT_FILES) {
    OUTPUT_FILES += "    <file>$$file</file>"
}

# Prepare output lines for qmlresources.qrc
LINES = "<RCC>" \
        "  <qresource prefix=\"/\">" \
        $$OUTPUT_FILES \
        "  </qresource>" \
        "</RCC>"

# Write the qmlresources.qrc file
write_file("qmlresources.qrc", LINES)

android {
    QT += androidextras

    HEADERS += statusbarhelper.h \
               androidimageprovider.h \
               androidprocessmodel.h
    SOURCES += statusbarhelper.cpp \
               androidprocessmodel.cpp \
               androidimageprovider.cpp

    OTHER_FILES += $$files($$PWD/android/src/*.java) \
                   android/AndroidManifest.xml
} else: ios {
    LIBS += -framework AdSupport
} else {
    QT += widgets

    HEADERS += desktopstatusbarhelper.h
    SOURCES += desktopstatusbarhelper.cpp

    linux {
        CONFIG += link_pkgconfig
        PKGCONFIG += libwnck-1.0

        HEADERS += linuximageprovider.h \
                   linuxprocessmodel.h
        SOURCES += linuximageprovider.cpp \
                   linuxprocessmodel.cpp
    }

    osx {
        QT += macextras
        LIBS += -framework AppKit -framework DiskArbitration

        HEADERS += macimageprovider.h \
                   macprocessmodel.h
        OBJECTIVE_SOURCES += macimageprovider.mm \
                             macprocessmodel.mm
    }
}

win32 {
    conf.files = qt.conf
    conf.path = $${target.path}
    INSTALLS += conf
}

osx {
    QT += macextras

    LIBS += -framework SystemConfiguration \
            -framework Foundation \
            -framework AppKit \
            -framework IOKit  \
            -framework QTKit \
            -framework CoreWLAN \
            -framework CoreLocation \
            -framework CoreFoundation \
            -framework ScreenSaver \
            -framework IOBluetooth \
            -framework CoreServices \
            -framework DiskArbitration \
            -framework ApplicationServices
}

!android:linux {
    CONFIG += link_pkgconfig
    PKGCONFIG += libwnck-1.0
    DEFINES += HAVE_WNCK
}

HEADERS += \
    units.h \
    qmlmodule.h

SOURCES += \
    main.cpp \
    units.cpp \
    qmlmodule.cpp

RESOURCES += \
    qmlresources.qrc \
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

DISTFILES += \
    qml/controls/LabeledComboBox.qml \
    qml/controls/LabeledTextField.qml

