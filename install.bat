@echo off
mkdir csdev
xcopy /E /Y .\lib .\csdev\lib\
xcopy /E /Y .\include .\csdev\include\
@echo on