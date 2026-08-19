@echo off
cd "%~dp0\.."
for /R .\include\ %%i in (*.h *.hpp *.cpp) do @clang-format -i %%i
for /R .\sources\ %%i in (*.h *.hpp *.cpp) do @clang-format -i %%i
for /R .\unit_tests\ %%i in (*.h *.hpp *.cpp) do @clang-format -i %%i
for /R .\tests\ %%i in (*.h *.hpp *.cpp) do @clang-format -i %%i
