#LIBS += -ldns_sd
#INCLUDEPATH +=

unix:!macx: LIBS += -L$$PWD/../../../../../usr/lib/x86_64-linux-gnu/ -ldns_sd

INCLUDEPATH += $$PWD/../../../../../usr/lib/x86_64-linux-gnu
DEPENDPATH += $$PWD/../../../../../usr/lib/x86_64-linux-gnu
