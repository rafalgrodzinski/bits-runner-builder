@echo off
setlocal enabledelayedexpansion
:: Change code page to UTF-8
chcp 65001 > nul

set "SCRIPT_DIR=%~dp0"
set SCRIPT_DIR=%SCRIPT_DIR:~0,-1%
goto :start

:check
    if %ERRORLEVEL% gtr 0 (
        echo ⛔️ %~1 failed
        exit 1
    )
    exit /b 0

:start
:: Setup Environment
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
if %ERRORLEVEL% gtr 0 exit /b

set "PATH=%SCRIPT_DIR%\..\build;%PATH%"

:: Build brb
echo 🤖 Building brb...
cmake -B "%SCRIPT_DIR%\..\build"
call :check "Building brb"
cmake --build "%SCRIPT_DIR%\..\build" --config RelWithDebInfo
call :check "Building brb"
echo:

:: Run tests
set /a PASSED_TESTS=0
set /a FAILED_TESTS=0

for /D %%T in ("%SCRIPT_DIR%\*") do (
    echo 🤖🤖 Running test %%~nxT...
    call "%SCRIPT_DIR%\%%~nxT\run.bat"
    if !ERRORLEVEL! gtr 0 (
        set /a FAILED_TESTS=!FAILED_TESTS! + 1
    ) else (
        set /a PASSED_TESTS=!PASSED_TESTS! + 1
    )
)

echo ✅ Passed tests: %PASSED_TESTS%
echo ⛔️ Failed tests: %FAILED_TESTS%