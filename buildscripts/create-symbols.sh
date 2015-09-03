#!/bin/bash -e

# goto source directory
cd "$(dirname "$0")"/..

# default to 0 if no build number is provided via first argument
BUILD="${1:-0}"
BUILD_DIR=/home/build
DUMP_SYMS=./3rdparty/breakpad/src/tools/linux/dump_syms/dump_syms
SYM_DIR=./symbols
# library name: libglimpse.so.<version>
LIBGLIMPSE="$(find "${BUILD_DIR}" -type f -name 'libglimpse.so.*')"
# glimpse_<version>-<build>-dbg.tar.gz
TARBALL="glimpse_$(sed -nr 's/libglimpse\.so\.(.+)/\1/p' <<< ${LIBGLIMPSE##*/})-${BUILD}-dbg.tar.gz"

# build dump_syms if necessary
if [[ ! -x "${DUMP_SYMS}" ]]; then
	cd 3rdparty/breakpad
	./configure
	make
	cd -
fi

files=(
"$(find "${BUILD_DIR}" -type f -name glimpse-gui)"
"$(find "${BUILD_DIR}" -type f -name glimpse-console)"
"${LIBGLIMPSE}"
)

if [[ "${#files[*]}" -eq 0 ]]; then
	echo "could not locate binaries" >&2
	exit 1
fi

# remove old symbols
rm -rf "${SYM_DIR}"

for f in ${files[*]}; do
    sym_file="${f##*/}.sym"
    # dump debug symbols
    ${DUMP_SYMS} "${f}" > "${sym_file}"
    # create directory structure: $SYM_DIR/<file>/<id>
    dir="${SYM_DIR}/${f##*/}/$(head -n1 "${sym_file}" | awk '{print $4}')"
    mkdir -p "${dir}"
    # move symbol file to correct directory
    mv "${sym_file}" "${dir}"
done

# create tarball
rm -f "${TARBALL}"
tar -czf "${TARBALL}" -C "${SYM_DIR}" .
