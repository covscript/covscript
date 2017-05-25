@del /F /Q .\sources\*.gch
@astyle .\sources\*.*
@astyle .\sources\extensions\*.*
@del /F /Q .\sources\*.orig
@del /F /Q .\sources\extensions\*.orig
@astyle -A4 -N -t .\sources\*.*
@astyle -A4 -N -t .\sources\extensions\*.*
@del /F /Q .\sources\*.orig
@del /F /Q .\sources\extensions\*.orig
