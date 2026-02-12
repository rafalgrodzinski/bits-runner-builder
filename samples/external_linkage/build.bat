@echo off

set "SCRIPT_DIR=%~dp0"
set "BRB_PATH=%SCRIPT_DIR%\..\..\build\Release\brb"

%BRB_PATH% "%SCRIPT_DIR%\io.brc" "%SCRIPT_DIR%\main.brc" "%SCRIPT_DIR%\stuff.brc"
cl /o external_linkage io.o stuff.o main.o msvcrt.lib