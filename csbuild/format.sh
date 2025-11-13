#!/usr/bin/env bash
function format() {
    file_list=$(ls $1)
    cd $1
    clang-format -i *.* &> /dev/null
    astyle -q *.* &> /dev/null
    rm *.orig &> /dev/null
    astyle -A4 -N -t -q *.* &> /dev/null
    rm *.orig &> /dev/null
    for file in $file_list;do
        if test -d ${file};then
            format ${file} "$2"
        fi
    done
    cd ..
}
CURRENT_FOLDER=$(dirname $(readlink -f "$0"))
cd $CURRENT_FOLDER/..
format ./include
format ./sources