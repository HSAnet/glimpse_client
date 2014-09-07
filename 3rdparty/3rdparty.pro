TEMPLATE = subdirs

SUBDIRS += miniupnp.pro

!win32:SUBDIRS += qtsystems.pro

OTHER_FILES += miniupnpc.pri \
               breakpad.pri \
               winpcap.pri \
               qtsystems.pri
