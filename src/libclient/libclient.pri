QT += network

INCLUDEPATH += $$PWD

win32 {
    CONFIG(release, debug|release):LIBS += -L../libclient/release
    else: LIBS += -L../libclient/debug

    LIBS += -lclient
}
else:LIBS += ../libclient/libclient.a

linux:!android:LIBS += -lminiupnpc
win32:LIBS += -lws2_32
