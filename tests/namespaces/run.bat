@echo off

:: Setup
set /a EXPECTED_VALUE=33

set "SCRIPT_DIR=%~dp0"
set "SCRIPT_DIR=%SCRIPT_DIR:~0,-1%"
call "%SCRIPT_DIR%\..\lib.bat" setup

:: Build test
brb "%SCRIPT_DIR%\main.brc" "%SCRIPT_DIR%\sub.brc"
call "%LIB_PATH%" check %ERRORLEVEL%
link /nologo /out:%NAME%.exe main.o sub.o msvcrt.lib
call "%LIB_PATH%" check %ERRORLEVEL% def

:: Run test 
".\%NAME%.exe"
call "%LIB_PATH%" check_test %ERRORLEVEL% %EXPECTED_VALUE% "%NAME%"