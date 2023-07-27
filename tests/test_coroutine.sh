#!/bin/bash
clear
for (( i = 1; i <= 1000; i = i + 1 )); do
  echo "system.console.gotoxy(0,0)" | cs STDIN
  echo $i | cs ./test_coroutine.csc
done
