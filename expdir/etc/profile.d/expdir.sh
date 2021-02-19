expdir()
{
    location="/var/cache/expdir/location"
    "/usr/local/bin/expdirapp" "$@"
    if test -f "${location}"; then
        dir="$(cat ${location})"
        cd "${dir}"
        rm "${location}"
    fi
}
