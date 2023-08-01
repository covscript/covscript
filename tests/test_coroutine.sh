#!/bin/bash
CURRENT_FOLDER=$(dirname $(readlink -f "$0"))
cd $CURRENT_FOLDER
clear
for (( i = 1; i <= 1000; i = i + 1 )); do
  echo $i | cs test_coroutine.csc
done
