TEMPLATE = subdirs

SUBDIRS += miniupnp.pro \
           ejdb.pro

!win32:SUBDIRS += qtsystems.pro

OTHER_FILES += miniupnpc.pri \
               breakpad.pri \
               winpcap.pri \
               qtsystems.pri \
               ejdb.pri
