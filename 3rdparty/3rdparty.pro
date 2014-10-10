TEMPLATE = subdirs

SUBDIRS += miniupnp.pro

!ios:SUBDIRS += breakpad.pro

!win32:!android:!ios:SUBDIRS += qtsystems.pro

OTHER_FILES += miniupnpc.pri \
               breakpad.pri \
               winpcap.pri \
               qtsystems.pri
