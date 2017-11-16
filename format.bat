@for /R .\include\ %%i in (*.gch) do @del /F /Q %%i
@for /R .\include\ %%i in (*.*) do @astyle %%i
@for /R .\include\ %%i in (*.orig) do @del /F /Q %%i
@for /R .\include\ %%i in (*.*) do @astyle -A4 -N -t %%i
@for /R .\include\ %%i in (*.orig) do @del /F /Q %%i
@for /R .\sources\ %%i in (*.gch) do @del /F /Q %%i
@for /R .\sources\ %%i in (*.*) do @astyle %%i
@for /R .\sources\ %%i in (*.orig) do @del /F /Q %%i
@for /R .\sources\ %%i in (*.*) do @astyle -A4 -N -t %%i
@for /R .\sources\ %%i in (*.orig) do @del /F /Q %%i
