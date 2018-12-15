@echo off
mkdir build
mkdir csdev
xcopy /E /Y .\bin .\build\bin\
xcopy /E /Y .\lib .\csdev\lib\
xcopy /E /Y .\include\covscript .\csdev\include\covscript\
@echo on