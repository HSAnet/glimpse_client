CONFIG += testcase
CONFIG -= app_bundle
QT += testlib

TARGET = tst_periodictiming
SOURCES = tst_periodictiming.cpp

include($$SOURCE_DIRECTORY/src/libclient/libclient.pri)
