TEMPLATE = subdirs

SUBDIRS += libclient

qtHaveModule(quick) {
    SUBDIRS += mobile
    mobile.depends = libclient
}

!android:!ios {
    SUBDIRS += console
    console.depends = libclient
}

tests {
    SUBDIRS += tests
    tests.depends = libclient
}
