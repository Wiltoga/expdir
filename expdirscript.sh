#!/bin/bash
location="/var/cache/expdir/location"
"/usr/local/bin/expdirapp" invoke "$@"
if test -f "${location}"; then
    dir="$(cat ${location})"
    cd "${dir}"
    rm "${location}"
fi
