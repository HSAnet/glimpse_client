TEMPLATE = app

# Add more folders to ship with the application, here
folder_01.source = qml
folder_01.target = ""
DEPLOYMENTFOLDERS = folder_01

qmlfiles.files = $$PWD/qml
qmlfiles.path = .
QMAKE_BUNDLE_DATA += qmlfiles

qmlimports.files = $$[QT_INSTALL_QML]
qmlimports.path = imports
QMAKE_BUNDLE_DATA += qmlimports

QT += gui quick qml concurrent

include(../libclient/libclient.pri)

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android
ANDROID_PACKAGE = "de.hsaugsburg.informatik.mplane.client"
ANDROID_APP_NAME = "mPlane"

android {
    QT += androidextras

    HEADERS += statusbarhelper.h \
               androidimageprovider.h \
               androidprocessmodel.h
    SOURCES += statusbarhelper.cpp \
               androidimageprovider.cpp \
               androidprocessmodel.cpp

    OTHER_FILES += $$files($$PWD/android/src/*.java) \
                   android/AndroidManifest.xml
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
    LIBS += -framework AppKit

    HEADERS += macprocessmodel.h \
               macimageprovider.h
    OBJECTIVE_SOURCES += macprocessmodel.mm \
                         macimageprovider.mm
}

SOURCES += \
    main.cpp

RESOURCES += \
    resources/resource.qrc

# Dump symbols
#symdump.commands = dump_syms $$qtLibraryTarget($$TARGET) 2>/dev/null > "$${TARGET}.sym"
#QMAKE_EXTRA_TARGETS += symdump
#PRE_TARGETDEPS += symdump

# Please do not modify the following two lines. Required for deployment.
include(qtquick2applicationviewer/qtquick2applicationviewer.pri)
qtcAddDeployment()

OTHER_FILES += \
    qml/main.qml \
    qml/packettrain.qml \
    qml/android/TextFieldStyle.qml \
    qml/android/TabViewStyle.qml \
    qml/android/SliderStyle.qml \
    qml/android/ProgressBarStyle.qml \
    qml/android/ButtonStyle.qml \
    qml/android/BackButton.qml \
    qml/android/AndroidDelegate.qml \
    qml/android/images/toolbar.png \
    qml/android/images/textinput.png \
    qml/android/images/tabs_standard.png \
    qml/android/images/tab_selected.png \
    qml/android/images/navigation_previous_item.png \
    qml/android/images/navigation_next_item.png \
    qml/android/images/button_pressed.png \
    qml/android/images/button_default.png \
    qml/Settings.qml \
    qml/Tests.qml \
    qml/android/Spinner.qml \
    qml/Card.qml \
    qml/CardListDelegate.qml \
    qml/speedtest.qml \
    qml/ResultPage.qml \
    qml/Slow.qml \
    qml/UserRegistration.qml \
    qml/DeviceRegistration.qml \
    qml/Login.qml
