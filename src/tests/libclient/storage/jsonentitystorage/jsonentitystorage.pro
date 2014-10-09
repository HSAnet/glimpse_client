CONFIG += testcase
CONFIG -= app_bundle
QT += testlib

TARGET = tst_jsonentitystorage
SOURCES = tst_jsonentitystorage.cpp

include($$SOURCE_DIRECTORY/src/libclient/libclient.pri)
