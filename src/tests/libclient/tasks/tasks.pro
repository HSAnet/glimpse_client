CONFIG += testcase
CONFIG -= app_bundle
QT += testlib

TARGET = tst_tasks
SOURCES = tst_tasks.cpp

include($$SOURCE_DIRECTORY/src/libclient/libclient.pri)
