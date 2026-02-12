@echo off

set "SCRIPT_DIR=%~dp0"
set "BRB_PATH=%SCRIPT_DIR%\..\..\build\Release\brb"

%BRB_PATH% "%SCRIPT_DIR%\io.brc" "%SCRIPT_DIR%\main.brc"
cl /o linked_list io.o main.o msvcrt.lib