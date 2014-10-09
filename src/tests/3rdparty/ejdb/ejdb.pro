CONFIG += testcase
CONFIG -= app_bundle
QT += testlib

TARGET = tst_ejdb
SOURCES = tst_ejdb.cpp

include($$SOURCE_DIRECTORY/3rdparty/ejdb.pri)
