expdir()
{
    dir="<install directory>/bin" #Change this variable, the directory of the executable file
    "${dir}/expdir" "$@"
    if test -f "${dir}/location"; then
        location="$(cat ${dir}/location)"
        cd "${location}"
        rm "${dir}/location"
    fi
}
