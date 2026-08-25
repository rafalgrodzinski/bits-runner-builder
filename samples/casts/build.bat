@echo off

set "SCRIPT_DIR=%~dp0"
set SCRIPT_DIR=%SCRIPT_DIR:~0,-1%
set "BRB_PATH=%SCRIPT_DIR%\..\..\build\brb"
set "B_DIR=%SCRIPT_DIR%\..\..\lib\B"

"%BRB_PATH%" "%SCRIPT_DIR%\main.brc" "%B_DIR%\String.brc" "%B_DIR%\BSys.brc"
if %ERRORLEVEL% gtr 0 exit

:: Setup MSVC Environment and link
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
if %ERRORLEVEL% gtr 0 exit

link /out:casts.exe main.o B.o BSys.o msvcrt.lib