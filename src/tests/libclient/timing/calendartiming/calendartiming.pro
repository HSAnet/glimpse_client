CONFIG += testcase
CONFIG -= app_bundle
QT += testlib

TARGET = tst_calendartming
SOURCES = tst_calendartiming.cpp

include($$SOURCE_DIRECTORY/src/libclient/libclient.pri)
