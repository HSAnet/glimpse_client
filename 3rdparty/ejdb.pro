TARGET = tcejdb
TEMPLATE = lib
CONFIG += staticlib
CONFIG -= qt

EJDB_DIR = $$PWD/ejdb/tcejdb

DEFINES += _MYNOZLIB

*gcc*:QMAKE_CFLAGS += -std=gnu99 -Wall -fsigned-char -Wfatal-errors

android {
    INCLUDEPATH += ejdb_android_fixes
    HEADERS += $$PWD/ejdb_android_fixes/glob.h
    SOURCES += $$PWD/ejdb_android_fixes/glob.c
}

# Information taken from ejdb/tcejdb/configure.ac

HEADERS += $$EJDB_DIR/tcutil.h \
           $$EJDB_DIR/tchdb.h \
           $$EJDB_DIR/tcbdb.h \
           $$EJDB_DIR/tcfdb.h \
           $$EJDB_DIR/tctdb.h \
           $$EJDB_DIR/tcadb.h \
           $$EJDB_DIR/ejdb.h \
           $$EJDB_DIR/ejdb_private.h \
           $$EJDB_DIR/bson.h \
           $$EJDB_DIR/myconf.h \
           $$EJDB_DIR/basedefs.h

SOURCES += $$EJDB_DIR/tcutil.c \
           $$EJDB_DIR/tchdb.c \
           $$EJDB_DIR/tcbdb.c \
           $$EJDB_DIR/tcfdb.c \
           $$EJDB_DIR/tctdb.c \
           $$EJDB_DIR/tcadb.c \
           $$EJDB_DIR/myconf.c \
           $$EJDB_DIR/md5.c \
           $$EJDB_DIR/ejdb.c \
           $$EJDB_DIR/bson.c \
           $$EJDB_DIR/numbers.c \
           $$EJDB_DIR/encoding.c \
           $$EJDB_DIR/utf8proc.c \
           $$EJDB_DIR/ejdbutl.c \
           $$EJDB_DIR/platform.c \
           $$EJDB_DIR/nxjson.c
