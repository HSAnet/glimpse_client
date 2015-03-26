CONFIG += testcase
CONFIG -= app_bundle
QT += testlib

TARGET = tst_onofftiming
SOURCES = tst_onofftiming.cpp

include($$SOURCE_DIRECTORY/src/libclient/libclient.pri)
