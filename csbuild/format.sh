#!/usr/bin/env bash
function format() {
    file_list=$(ls $1)
    cd $1
    clang-format -i *.* &> /dev/null
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
