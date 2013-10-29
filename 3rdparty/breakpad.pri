BREAKPAD_PATH = $$PWD/breakpad

INCLUDEPATH += $$BREAKPAD_PATH \
               $$BREAKPAD_PATH/src

linux {
    HEADERS += $$BREAKPAD_PATH/src/client/linux/handler/exception_handler.h
    HEADERS += $$BREAKPAD_PATH/src/client/linux/crash_generation/crash_generation_client.h
    HEADERS += $$BREAKPAD_PATH/src/client/linux/handler/minidump_descriptor.h
    HEADERS += $$BREAKPAD_PATH/src/client/linux/minidump_writer/minidump_writer.h
    HEADERS += $$BREAKPAD_PATH/src/client/linux/minidump_writer/line_reader.h
    HEADERS += $$BREAKPAD_PATH/src/client/linux/minidump_writer/linux_dumper.h
    HEADERS += $$BREAKPAD_PATH/src/client/linux/minidump_writer/linux_ptrace_dumper.h
    HEADERS += $$BREAKPAD_PATH/src/client/linux/minidump_writer/directory_reader.h
    HEADERS += $$BREAKPAD_PATH/src/client/linux/log/log.h
    HEADERS += $$BREAKPAD_PATH/src/client/minidump_file_writer-inl.h
    HEADERS += $$BREAKPAD_PATH/src/client/minidump_file_writer.h
    HEADERS += $$BREAKPAD_PATH/src/common/linux/linux_libc_support.h
    HEADERS += $$BREAKPAD_PATH/src/common/linux/eintr_wrapper.h
    HEADERS += $$BREAKPAD_PATH/src/common/linux/ignore_ret.h
    HEADERS += $$BREAKPAD_PATH/src/common/linux/file_id.h
    HEADERS += $$BREAKPAD_PATH/src/common/linux/memory_mapped_file.h
    HEADERS += $$BREAKPAD_PATH/src/common/linux/safe_readlink.h
    HEADERS += $$BREAKPAD_PATH/src/common/linux/guid_creator.h
    HEADERS += $$BREAKPAD_PATH/src/common/linux/elfutils.h
    HEADERS += $$BREAKPAD_PATH/src/common/linux/elfutils-inl.h
    HEADERS += $$BREAKPAD_PATH/src/common/using_std_string.h
    HEADERS += $$BREAKPAD_PATH/src/common/memory.h
    HEADERS += $$BREAKPAD_PATH/src/common/basictypes.h
    HEADERS += $$BREAKPAD_PATH/src/common/memory_range.h
    HEADERS += $$BREAKPAD_PATH/src/common/string_conversion.h
    HEADERS += $$BREAKPAD_PATH/src/common/convert_UTF.h
    HEADERS += $$BREAKPAD_PATH/src/google_breakpad/common/minidump_format.h
    HEADERS += $$BREAKPAD_PATH/src/google_breakpad/common/minidump_size.h
    HEADERS += $$BREAKPAD_PATH/src/google_breakpad/common/breakpad_types.h
    HEADERS += $$BREAKPAD_PATH/src/third_party/lss/linux_syscall_support.h
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

