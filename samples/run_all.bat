@echo off
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

:: Build brb
echo 🤖 Building brb...
cmake -B "%SCRIPT_DIR%\..\build"
call :check "Building brb"
cmake --build "%SCRIPT_DIR%\..\build" --config RelWithDebInfo
call :check "Building brb"
echo:

:: Build and run samples
for /D %%S in ("%SCRIPT_DIR%\*") do (
    :: Build sample
    echo 🐷 Building %%~nxS
    call "%SCRIPT_DIR%\%%~nxS\build.bat"
    call :check "Building %%~nxS"
    :: Run sample
    echo 🐷🐷 Running %%~nxS
    "%%~nxS.exe"
    call :check "%%~nxS"

    echo:
)