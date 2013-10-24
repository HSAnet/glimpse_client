TEMPLATE = app

# Add more folders to ship with the application, here
folder_01.source = qml
folder_01.target = ""
DEPLOYMENTFOLDERS = folder_01

QT += gui quick qml concurrent

include(../libclient/libclient.pri)

android {
    HEADERS += statusbarhelper.h \
               androidimageprovider.h \
               androidprocessmodel.h
    SOURCES += statusbarhelper.cpp \
               androidimageprovider.cpp \
               androidprocessmodel.cpp
} else {
    QT += widgets

    HEADERS += desktopstatusbarhelper.h
    SOURCES += desktopstatusbarhelper.cpp
}

SOURCES += \
    main.cpp

RESOURCES += \
    resources/resource.qrc

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
