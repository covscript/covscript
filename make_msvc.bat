@mkdir build
@cd build
@mkdir bin
@rc /nologo /i ..\include /fo cs.res ..\sources\win32_rc\standalone.rc
@rc /nologo /i ..\include /fo cs_repl.res ..\sources\win32_rc\repl.rc
@cl /nologo /I ..\include /O2 /EHsc /std:c++14 /utf-8 /w ..\sources\standalone.cpp .\cs.res /Fe:.\bin\cs.exe
@cl /nologo /I ..\include /O2 /EHsc /std:c++14 /utf-8 /w ..\sources\repl.cpp .\cs_repl.res /Fe:.\bin\cs_repl.exe
@del /F /Q *.res
@del /F /Q *.obj