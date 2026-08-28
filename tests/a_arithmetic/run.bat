@echo off

set "SCRIPT_DIR=%~dp0"
set SCRIPT_DIR=%SCRIPT_DIR:~0,-1%
for %%N in ("%SCRIPT_DIR%") do set "NAME=%%~nxN"
goto :start

:check
    if %~1 gtr 0 (
        if not "%~2" == "" (
            echo ⛔️ %~2 failed
        )
        exit %~1
    )
    exit /b 0

:check_test
    if %~1 == %~2 (
        echo ✅ Test %~3 Passed
        exit /b 0
    ) else (
        echo ⛔️ Test %~3 Failed
        exit /b 1
    )

:start
brb "%SCRIPT_DIR%\main.brc"
call :check %ERRORLEVEL%
link /out:%NAME%.exe main.o msvcrt.lib
call :check %ERRORLEVEL%
".\%NAME%.exe"
call :check_test %ERRORLEVEL% 7 "%NAME%"