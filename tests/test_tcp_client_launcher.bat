@echo off
for /L %%i in (1,1,20) do start cs .\test_tcp_client.csc %%i%
