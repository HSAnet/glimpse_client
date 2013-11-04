TEMPLATE = subdirs

!ios:mac|linux:SUBDIRS += miniupnp.pro

OTHER_FILES += miniupnpc.pri \
               breakpad.pri
