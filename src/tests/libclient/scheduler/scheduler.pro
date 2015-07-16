CONFIG += testcase
CONFIG -= app_bundle
QT += testlib

TARGET = tst_scheduler
SOURCES = tst_scheduler.cpp

include($$SOURCE_DIRECTORY/src/libclient/libclient.pri)
