QT += network

INCLUDEPATH += $$PWD

win32:LIBS += -L../libclient -lclient
else:LIBS += ../libclient/libclient.a

linux:!android:LIBS += -lminiupnpc
win32:LIBS += -lws2_32
