@echo off

set "SCRIPT_DIR=%~dp0"
set SCRIPT_DIR=%SCRIPT_DIR:~0,-1%
set "BRB_PATH=%SCRIPT_DIR%\..\..\build\brb"
set "LIB_DIR=%SCRIPT_DIR%\..\..\lib\brc"

"%BRB_PATH%" "%SCRIPT_DIR%\main.brc" "%LIB_DIR%\B\String.brc" "%LIB_DIR%\B\Date.brc" "%LIB_DIR%\BSys.brc"
if %ERRORLEVEL% gtr 0 exit /b

:: Setup MSVC Environment and link
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
if %ERRORLEVEL% gtr 0 exit /b

link /out:date.exe main.o B.o BSys.o msvcrt.lib