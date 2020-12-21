expdir()
{
    dir="<install directory>" #Change this variable
    "${dir}/expdir" "$@"
    if test -f "${dir}/location"; then
        location="$(cat ${dir}/location)"
        cd "${location}"
        rm "${dir}/location"
    fi
}