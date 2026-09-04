@echo off

if "%~1"=="setup" goto :setup
if "%~1"=="check" goto :check
if "%~1"=="check_test" goto :check_test
exit /b 0

:setup
    for %%N in ("%SCRIPT_DIR%") do set "NAME=%%~nxN"
    set "LIB_PATH=%SCRIPT_DIR%\..\lib.bat"
    exit /b 0

:check
    if %~2 gtr 0 (
        if not "%~3" == "" (
            echo ⛔️ %~3 failed
        )
        exit %~2
    )
    exit /b 0

:check_test
    if %~2 == %~3 (
        echo ✅ Test %~4 Passed
        exit /b 0
    ) else (
        echo ⛔️ Test %~4 Failed
        exit /b 1
    )