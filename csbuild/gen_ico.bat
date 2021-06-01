@echo off
cd %~dp0\..\docs
convert .\covariant_script.png -define icon:auto-resize="256,128,96,64,48,32,16" .\covariant_script.ico
convert .\covariant_script_debugger.png -define icon:auto-resize="256,128,96,64,48,32,16" .\covariant_script_debugger.ico
convert .\covariant_script_interpreter.png -define icon:auto-resize="256,128,96,64,48,32,16" .\covariant_script_interpreter.ico