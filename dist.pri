win32 {
} else: osx {
    target.path = /
    #INSTALLS *= target
} else: ios {
} else: android {
} else: linux {
    # Set the rpath, otherwise we can't load the client applications without changing LD_LIBRARY_PATH
    QMAKE_LFLAGS += -Wl,-z,origin \'-Wl,-rpath,\$\$ORIGIN\'

    target.path = /opt/glimpse
    INSTALLS *= target
} else {
    error(No distribution for this platform defined)
}

# Setup universal translations
trs.path = $${target.path}/locales

for(locale, TRANSLATIONS) {
    qmfile = $$locale
    qmfile ~= s/\.ts/.qm/i

    trs.files += qmfile

    # Add only once but don't add if we don't run in the loop
    # otherwise make will throw errors
    INSTALLS *= trs
}
