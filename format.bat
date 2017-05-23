@del /F /Q .\sources\*.gch
@astyle .\sources\*.*
@astyle .\sources\*\*.*
@del /F /Q .\sources\*.orig
@del /F /Q .\sources\*\*.orig
@astyle -A4 -N -t .\sources\*.*
@astyle -A4 -N -t .\sources\*\*.*
@del /F /Q .\sources\*.orig
@del /F /Q .\sources\*\*.orig
