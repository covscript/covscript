#!/usr/bin/env bash
trap 'onCtrlC' INT
function onCtrlC ()
{
    echo 'Ctrl+C is captured'
}
for f in *.csc
do
    echo Testing $f...
    cs --wait-before-exit $f
done
