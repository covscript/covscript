@mkdir build
@cd build
@mkdir bin
@mkdir lib
@rc /nologo /i ..\include /fo dll.res ..\sources\covscript.rc
@rc /nologo /i ..\include /fo cs.res ..\sources\standalone.rc
@rc /nologo /i ..\include /fo cs_repl.res ..\sources\repl.rc
@cl /nologo /I ..\include /O2 /favor:blend /EHac /std:c++14 /utf-8 /w ..\sources\standalone.cpp .\cs.res /Fe:.\bin\cs.exe
@cl /nologo /I ..\include /O2 /favor:blend /EHac /std:c++14 /utf-8 /w ..\sources\repl.cpp .\cs_repl.res /Fe:.\bin\cs_repl.exe
@cl /nologo /I ..\include /O2 /favor:blend /EHac /std:c++14 /utf-8 /w /LD ..\sources\covscript.cpp .\dll.res /Fe:.\lib\libcovscript.dll
@del /F /Q *.res
@del /F /Q *.obj