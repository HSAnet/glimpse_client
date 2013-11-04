BREAKPAD_PATH = $$PWD/breakpad

INCLUDEPATH += $$BREAKPAD_PATH \
               $$BREAKPAD_PATH/src

win32 {
    CONFIG += breakpad-builtin

    SOURCES += $$BREAKPAD_PATH/src/client/windows/handler/exception_handler.cc
    SOURCES += $$BREAKPAD_PATH/src/common/windows/string_utils.cc
    SOURCES += $$BREAKPAD_PATH/src/common/windows/guid_string.cc
    SOURCES += $$BREAKPAD_PATH/src/client/windows/crash_generation/crash_generation_client.cc
}

android {
    CONFIG += breakpad-builtin

    INCLUDEPATH += $$BREAKPAD_PATH/src/common/android/include

    SOURCES += $$BREAKPAD_PATH/src/client/linux/crash_generation/crash_generation_client.cc \
               $$BREAKPAD_PATH/src/client/linux/handler/exception_handler.cc \
               $$BREAKPAD_PATH/src/client/linux/handler/minidump_descriptor.cc \
               $$BREAKPAD_PATH/src/client/linux/log/log.cc \
               $$BREAKPAD_PATH/src/client/linux/minidump_writer/linux_dumper.cc \
               $$BREAKPAD_PATH/src/client/linux/minidump_writer/linux_ptrace_dumper.cc \
               $$BREAKPAD_PATH/src/client/linux/minidump_writer/minidump_writer.cc \
               $$BREAKPAD_PATH/src/client/minidump_file_writer.cc \
               $$BREAKPAD_PATH/src/common/android/breakpad_getcontext.S \
               $$BREAKPAD_PATH/src/common/convert_UTF.c \
               $$BREAKPAD_PATH/src/common/md5.cc \
               $$BREAKPAD_PATH/src/common/string_conversion.cc \
               $$BREAKPAD_PATH/src/common/linux/elfutils.cc \
               $$BREAKPAD_PATH/src/common/linux/file_id.cc \
               $$BREAKPAD_PATH/src/common/linux/guid_creator.cc \
               $$BREAKPAD_PATH/src/common/linux/linux_libc_support.cc \
               $$BREAKPAD_PATH/src/common/linux/memory_mapped_file.cc \
               $$BREAKPAD_PATH/src/common/linux/safe_readlink.cc
}

linux:!android {
    CONFIG += breakpad-builtin

    SOURCES += $$BREAKPAD_PATH/src/client/linux/crash_generation/crash_generation_client.cc
    SOURCES += $$BREAKPAD_PATH/src/client/linux/handler/exception_handler.cc
    SOURCES += $$BREAKPAD_PATH/src/client/linux/handler/minidump_descriptor.cc
    SOURCES += $$BREAKPAD_PATH/src/client/linux/minidump_writer/minidump_writer.cc
    SOURCES += $$BREAKPAD_PATH/src/client/linux/minidump_writer/linux_dumper.cc
    SOURCES += $$BREAKPAD_PATH/src/client/linux/minidump_writer/linux_ptrace_dumper.cc
    SOURCES += $$BREAKPAD_PATH/src/client/linux/log/log.cc
    SOURCES += $$BREAKPAD_PATH/src/client/minidump_file_writer.cc
    SOURCES += $$BREAKPAD_PATH/src/common/linux/linux_libc_support.cc
    SOURCES += $$BREAKPAD_PATH/src/common/linux/file_id.cc
    SOURCES += $$BREAKPAD_PATH/src/common/linux/memory_mapped_file.cc
    SOURCES += $$BREAKPAD_PATH/src/common/linux/safe_readlink.cc
    SOURCES += $$BREAKPAD_PATH/src/common/linux/guid_creator.cc
    SOURCES += $$BREAKPAD_PATH/src/common/linux/elfutils.cc
    SOURCES += $$BREAKPAD_PATH/src/common/string_conversion.cc
    SOURCES += $$BREAKPAD_PATH/src/common/convert_UTF.c

    QMAKE_CXXFLAGS += -g
}

ios {
    warning(No breakpad yet for ios)
}

osx {
    CONFIG += breakpad-builtin

    SOURCES += $$BREAKPAD_PATH/src/client/mac/crash_generation/crash_generation_client.cc
    SOURCES += $$BREAKPAD_PATH/src/client/mac/handler/exception_handler.cc
    SOURCES += $$BREAKPAD_PATH/src/client/mac/handler/minidump_generator.cc
    SOURCES += $$BREAKPAD_PATH/src/client/mac/handler/dynamic_images.cc
    SOURCES += $$BREAKPAD_PATH/src/client/mac/handler/breakpad_nlist_64.cc
    SOURCES += $$BREAKPAD_PATH/src/client/minidump_file_writer.cc
    SOURCES += $$BREAKPAD_PATH/src/common/mac/string_utilities.cc
    SOURCES += $$BREAKPAD_PATH/src/common/mac/file_id.cc
    SOURCES += $$BREAKPAD_PATH/src/common/mac/macho_id.cc
    SOURCES += $$BREAKPAD_PATH/src/common/mac/macho_walker.cc
    SOURCES += $$BREAKPAD_PATH/src/common/mac/macho_utilities.cc
    SOURCES += $$BREAKPAD_PATH/src/common/mac/bootstrap_compat.cc
    SOURCES += $$BREAKPAD_PATH/src/common/string_conversion.cc
    SOURCES += $$BREAKPAD_PATH/src/common/convert_UTF.c
    SOURCES += $$BREAKPAD_PATH/src/common/md5.cc

    OBJECTIVE_SOURCES += $$BREAKPAD_PATH/src/common/mac/MachIPC.mm

    QMAKE_CXXFLAGS += -g
}

breakpad-builtin {
    DEFINES += HAVE_BREAKPAD
}
