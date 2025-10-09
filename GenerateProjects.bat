@echo off
setlocal

:: --- Change working directory to the folder where this BAT is located (repo root) ---
cd /d "%~dp0"

:: --- Setup paths ---
set PREMAKE_EXE=Dependencies\premake\premake5.exe
set SCRIPT_DIR=Scripts

:: --- Generate Visual Studio solution ---
echo Generating Visual Studio 2022 solution...
call "%PREMAKE_EXE%" vs2022 --scripts="%SCRIPT_DIR%"

echo.
echo -----------------------------
echo Done! Solution generated.
echo -----------------------------

endlocal
