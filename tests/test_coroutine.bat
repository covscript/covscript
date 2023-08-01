@echo off
cd %~dp0
cls
for /L %%i in (1,1,1000) do echo %%i% | cs test_coroutine.csc
