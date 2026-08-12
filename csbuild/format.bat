@echo off
cd "%~dp0\.."
for /R .\include\ %%i in (*.*) do @clang-format -i %%i
for /R .\sources\ %%i in (*.*) do @clang-format -i %%i
