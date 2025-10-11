@echo off
setlocal

:: --- Change working directory to the folder where this BAT is located (repo root) ---
cd /d "%~dp0"

:: --- Setup paths ---
set PREMAKE_EXE=Dependencies\premake\premake5.exe
set SCRIPT_DIR=Scripts

:: --- install custom Visual Studio templates ---
echo Installing Visual Studio templates...
call "%PREMAKE_EXE%" install-templates --scripts="%SCRIPT_DIR%"

echo.
echo ----------------------------------------------
echo Done! Custom templates have been installed.
echo ----------------------------------------------

pause
endlocal