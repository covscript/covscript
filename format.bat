@for /R .\include\ %%i in (*.*) do @astyle -q %%i
@for /R .\include\ %%i in (*.orig) do @del /F /Q %%i
@for /R .\include\ %%i in (*.*) do @astyle -A4 -N -t -q %%i
@for /R .\include\ %%i in (*.orig) do @del /F /Q %%i
@for /R .\sources\ %%i in (*.*) do @astyle -q %%i
@for /R .\sources\ %%i in (*.orig) do @del /F /Q %%i
@for /R .\sources\ %%i in (*.*) do @astyle -A4 -N -t -q %%i
@for /R .\sources\ %%i in (*.orig) do @del /F /Q %%i
