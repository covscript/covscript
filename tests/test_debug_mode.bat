@echo off
rem Verify the COVSCRIPT_DEBUG runtime guard levels (Windows).
rem Runs test_debug_mode.csc (a fixture that abandons a suspended fiber) under
rem each mode and checks exit code and stderr.
rem
rem Usage:
rem   test_debug_mode.bat           uses `cs` on PATH
rem   set CS=c:\path\to\cs.exe  then test_debug_mode.bat

setlocal enabledelayedexpansion
cd /d "%~dp0"
if "%CS%"=="" set "CS=cs"

set "FAIL=0"

rem none: exit 0, no warning
set "COVSCRIPT_DEBUG=none"
"%CS%" test_debug_mode.csc >nul 2>_dbg_err.txt
set "EL=!ERRORLEVEL!"
if not "!EL!"=="0" (echo FAIL none-exit & set /a FAIL+=1)
findstr /c:"[fiber] warning" _dbg_err.txt >nul
if not errorlevel 1 (echo FAIL none-warn & set /a FAIL+=1)

rem warning: exit 0, warning
set "COVSCRIPT_DEBUG=warning"
"%CS%" test_debug_mode.csc >nul 2>_dbg_err.txt
set "EL=!ERRORLEVEL!"
if not "!EL!"=="0" (echo FAIL warning-exit & set /a FAIL+=1)
findstr /c:"[fiber] warning" _dbg_err.txt >nul
if errorlevel 1 (echo FAIL warning-warn & set /a FAIL+=1)

rem strict: nonzero exit (abort), warning
set "COVSCRIPT_DEBUG=strict"
"%CS%" test_debug_mode.csc >nul 2>_dbg_err.txt
set "EL=!ERRORLEVEL!"
if "!EL!"=="0" (echo FAIL strict-exit & set /a FAIL+=1)
findstr /c:"[fiber] warning" _dbg_err.txt >nul
if errorlevel 1 (echo FAIL strict-warn & set /a FAIL+=1)

rem unset: defaults to warning
set "COVSCRIPT_DEBUG="
"%CS%" test_debug_mode.csc >nul 2>_dbg_err.txt
set "EL=!ERRORLEVEL!"
if not "!EL!"=="0" (echo FAIL default-exit & set /a FAIL+=1)
findstr /c:"[fiber] warning" _dbg_err.txt >nul
if errorlevel 1 (echo FAIL default-warn & set /a FAIL+=1)

rem invalid value: defaults to warning
set "COVSCRIPT_DEBUG=bogus"
"%CS%" test_debug_mode.csc >nul 2>_dbg_err.txt
set "EL=!ERRORLEVEL!"
if not "!EL!"=="0" (echo FAIL invalid-exit & set /a FAIL+=1)
findstr /c:"[fiber] warning" _dbg_err.txt >nul
if errorlevel 1 (echo FAIL invalid-warn & set /a FAIL+=1)

del _dbg_err.txt
if not "%FAIL%"=="0" (echo fail=%FAIL% & exit /b 1)
echo PASS all COVSCRIPT_DEBUG levels
