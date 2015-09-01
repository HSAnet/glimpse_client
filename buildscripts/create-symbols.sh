#!/bin/bash -ex

DUMP_SYMS="$1"/3rdparty/breakpad/src/tools/linux/dump_syms/dump_syms
SYM_DIR="$1"/symbols
TARBALL="$1"/glimpse-dbg.tar.gz

files=(
"$(find $1 -type f -name glimpse-gui)"
"$(find $1 -type f -name glimpse-console)"
"$(find $1 -type f -name 'libglimpse.*')"
)

# remove old symbols
rm -rf "${SYM_DIR}"

for f in ${files[*]}; do
    sym_file=/tmp/"${f##*/}".sym
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
