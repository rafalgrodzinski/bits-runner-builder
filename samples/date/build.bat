@echo off

set "SCRIPT_DIR=%~dp0"
set "BRB_PATH=%SCRIPT_DIR%\..\..\build\Release\brb"
set "B_DIR=%SCRIPT_DIR%\..\..\lib\B"

%BRB_PATH% "%SCRIPT_DIR%\main.brc" "%B_DIR%\String.brc" "%B_DIR%\Date.brc" "%B_DIR%\BSys.brc"
cl /o date main.o B.o BSys.o msvcrt.lib