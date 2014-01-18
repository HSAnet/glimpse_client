TEMPLATE = subdirs

strings.commands = cd $$PWD/miniupnp/miniupnpc && sh updateminiupnpcstrings.sh
QMAKE_EXTRA_TARGETS += strings
PRE_TARGETDEPS += strings
