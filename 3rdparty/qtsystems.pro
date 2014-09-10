TEMPLATE = lib

!win32 {
    dynamic_link.target = $$PWD/qtsystems_fixes/QtSystemInfo
    dynamic_link.commands = ln -s $$PWD/qtsystems/src/systeminfo $$PWD/qtsystems_fixes/QtSystemInfo
#    dynamic_link.depends = FORCE

    QMAKE_EXTRA_TARGETS += dynamic_link
    PRE_TARGETDEPS += $$PWD/qtsystems_fixes/QtSystemInfo
}
