@echo off

set "SCRIPT_DIR=%~dp0"
set SCRIPT_DIR=%SCRIPT_DIR:~0,-1%
set "BRB_PATH=%SCRIPT_DIR%\..\..\build\brb"

"%BRB_PATH%" "%SCRIPT_DIR%\stuff.brc"
if %ERRORLEVEL% gtr 0 exit /b
"%BRB_PATH%" "%SCRIPT_DIR%\main.brc" "%SCRIPT_DIR%\io.brc"
if %ERRORLEVEL% gtr 0 exit /b

:: Setup MSVC Environment and link
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
if %ERRORLEVEL% gtr 0 exit /b

link /out:external_linkage.exe main.o io.o stuff.o msvcrt.lib