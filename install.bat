@echo off
mkdir build
mkdir csdev
xcopy /E /Y .\bin .\build\bin\
xcopy /E /Y .\lib .\csdev\lib\
xcopy /E /Y .\include .\csdev\include\
@echo on