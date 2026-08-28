@echo off

:: Setup
set /a EXPECTED_VALUE=21

set "SCRIPT_DIR=%~dp0"
set "SCRIPT_DIR=%SCRIPT_DIR:~0,-1%"
call "%SCRIPT_DIR%\..\lib.bat" setup

:: Build test
brb "%SCRIPT_DIR%\main.brc"
call "%LIB_PATH%" check %ERRORLEVEL%
link /nologo /out:%NAME%.exe main.o msvcrt.lib
call "%LIB_PATH%" check %ERRORLEVEL% def

:: Run test 
".\%NAME%.exe"
call "%LIB_PATH%" check_test %ERRORLEVEL% %EXPECTED_VALUE% "%NAME%"