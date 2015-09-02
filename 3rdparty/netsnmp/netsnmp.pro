#-------------------------------------------------
#
# Project created by QtCreator 2015-07-17T11:29:00
#
#-------------------------------------------------

QT       -= core gui

TARGET = netsnmp
TEMPLATE = lib
CONFIG += staticlib

DEFINES += OPENSSL_NO_INLINE_ASM \
            NETSNMP_DEFAULT_MIBDIRS=\\\"$$PWD/mibs\\\"

# Disable warnings
*clang*|*g++*|*llvm* {
    QMAKE_CFLAGS += -w
    QMAKE_CXXFLAGS += -w
}

INCLUDEPATH += $$PWD/include \
                $$PWD/snmplib
linux: INCLUDEPATH += $$PWD/debian-config
mac: INCLUDEPATH += $$PWD/mac-config
win32: INCLUDEPATH += $$PWD/win-config

SOURCES += snmplib/snmp_client.c \
           snmplib/mib.c \
           snmplib/parse.c \
           snmplib/snmp_api.c \
           snmplib/snmp.c \
           snmplib/snmp_auth.c \
           snmplib/asn1.c \
           snmplib/md5.c \
           snmplib/snmp_parse_args.c \
           snmplib/system.c \
           snmplib/vacm.c \
           snmplib/int64.c \
           snmplib/read_config.c \
           snmplib/snmp_debug.c \
           snmplib/tools.c \
           snmplib/snmp_logging.c \
           snmplib/large_fd_set.c \
           snmplib/snmpv3.c \
           snmplib/lcd_time.c \
           snmplib/keytools.c \
           snmplib/scapi.c \
           snmplib/callback.c \
           snmplib/default_store.c \
           snmplib/snmp_alarm.c \
           snmplib/data_list.c \
           snmplib/oid_stash.c \
           snmplib/fd_event_manager.c \
           snmplib/check_varbind.c \
           snmplib/mt_support.c \
           snmplib/snmp_enum.c \
           snmplib/snmp-tc.c \
           snmplib/snmp_service.c \
           snmplib/snmp_transport.c \
           snmplib/transports/snmpUDPBaseDomain.c \
           snmplib/transports/snmpUDPIPv4BaseDomain.c \
           snmplib/transports/snmpTCPBaseDomain.c \
           snmplib/transports/snmpSocketBaseDomain.c \
           snmplib/transports/snmpIPv4BaseDomain.c \
           snmplib/transports/snmpUDPDomain.c \
           snmplib/transports/snmpTCPDomain.c \
           snmplib/transports/snmpAliasDomain.c \
           snmplib/transports/snmpCallbackDomain.c \
           snmplib/snmp_secmod.c \
           snmplib/snmpusm.c \
           snmplib/snmp_version.c \
           snmplib/container_null.c \
           snmplib/container_list_ssll.c \
           snmplib/container_iterator.c \
           snmplib/ucd_compat.c \
           snmplib/container.c \
           snmplib/container_binary_array.c \
           snmplib/getopt.c
win32: {
SOURCES += \
    snmplib/closedir.c \
    snmplib/gettimeofday.c \
    snmplib/inet_ntop.c \
    snmplib/inet_pton.c \
    snmplib/opendir.c \
    snmplib/readdir.c \
    snmplib/snmp.c \
    snmplib/snmp_auth.c \
    snmplib/strlcat.c \
    snmplib/strlcpy.c \
    snmplib/strtoull.c \
    snmplib/winpipe.c \
    snmplib/strtok_r.c
}
linux: {
SOURCES += \
    snmplib/strlcat.c \
    snmplib/strlcpy.c
}
unix: {
SOURCES += \
           snmplib/transports/snmpUnixDomain.c \
           snmplib/text_utils.c \
           snmplib/cert_util.c \
           snmplib/pkcs.c \
           snmplib/dir_utils.c \
           snmplib/snprintf.c \
           snmplib/file_utils.c \
           snmplib/snmp_openssl.c \
           snmplib/openssl/openssl_aes_cfb.c \
           snmplib/openssl/openssl_aes_core.c \
           snmplib/openssl/openssl_cbc_enc.c \
           snmplib/openssl/openssl_cfb128.c \
           snmplib/openssl/openssl_des_enc.c \
           snmplib/openssl/openssl_md5.c \
           snmplib/openssl/openssl_ncbc_enc.c \
           snmplib/openssl/openssl_set_key.c \
           snmplib/openssl/openssl_sha1.c
}

HEADERS += \
    include/net-snmp/library/asn1.h \
    include/net-snmp/library/callback.h \
    include/net-snmp/library/container_binary_array.h \
    include/net-snmp/library/container_iterator.h \
    include/net-snmp/library/container_list_ssll.h \
    include/net-snmp/library/container_null.h \
    include/net-snmp/library/container.h \
    include/net-snmp/library/data_list.h \
    include/net-snmp/library/default_store.h \
    include/net-snmp/library/fd_event_manager.h \
    include/net-snmp/library/getopt.h \
    include/net-snmp/library/int64.h \
    include/net-snmp/library/keytools.h \
    include/net-snmp/library/large_fd_set.h \
    include/net-snmp/library/lcd_time.h \
    include/net-snmp/library/md5.h \
    include/net-snmp/library/mib.h \
    include/net-snmp/library/mt_support.h \
    include/net-snmp/library/oid_stash.h \
    include/net-snmp/library/parse.h \
    include/net-snmp/library/read_config.h \
    include/net-snmp/library/scapi.h \
    include/net-snmp/library/snmp_alarm.h \
    include/net-snmp/library/snmp_api.h \
    include/net-snmp/library/snmp_client.h \
    include/net-snmp/library/snmp_debug.h \
    include/net-snmp/library/snmp_logging.h \
    include/net-snmp/library/snmp_parse_args.h \
    include/net-snmp/library/snmp_service.h \
    include/net-snmp/library/snmp-tc.h \
    include/net-snmp/library/snmp.h \
    include/net-snmp/library/snmpv3.h \
    include/net-snmp/library/system.h \
    include/net-snmp/library/tools.h \
    include/net-snmp/library/snmpSocketBaseDomain.h \
    include/net-snmp/library/snmpTCPBaseDomain.h \
    include/net-snmp/library/snmpUDPBaseDomain.h \
    include/net-snmp/library/snmpUDPIPv4BaseDomain.h \
    include/net-snmp/library/snmpAliasDomain.h \
    include/net-snmp/library/snmpCallbackDomain.h \
    include/net-snmp/library/snmpIPv4BaseDomain.h \
    include/net-snmp/library/snmpTCPDomain.h \
    include/net-snmp/library/snmpUDPDomain.h \
    include/net-snmp/library/snmpusm.h \
    include/net-snmp/library/snmp_enum.h \
    include/net-snmp/library/check_varbind.h \
    include/net-snmp/library/snmp_secmod.h \
    include/net-snmp/library/ucd-compat.h \
    include/net-snmp/library/vcam.h \
    include/net-snmp/library/snmp-transport.h
win32: {
HEADERS += include/net-snmp/library/strtok_r.h
}
unix: {
HEADERS += \
    include/net-snmp/library/snmpUnixDomain.h \
    include/net-snmp/library/transform_oids.h \
    include/net-snmp/library/types.h \
    include/net-snmp/library/snmp_impl.h \
    include/net-snmp/library/snmp_assert.h \
    include/net-snmp/library/oid.h \
    include/net-snmp/library/factory.h \
    include/net-snmp/library/cert_util.h \
    include/net-snmp/library/dir_utils.h \
    include/net-snmp/library/file_utils.h \
    include/net-snmp/library/text_utils.h \
}
