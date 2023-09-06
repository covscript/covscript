#!/bin/bash
for (( i = 1; i <= 20; i = i + 1 )); do
  cs ./test_tcp_client.csc $i --pure &
done
wait
