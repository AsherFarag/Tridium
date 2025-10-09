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

:: --- Install custom Visual Studio templates ---
echo Installing Visual Studio templates...
call "%PREMAKE_EXE%" install-templates --scripts="%SCRIPT_DIR%"

echo.
echo -----------------------------
echo Done! Templates and solution generated.
echo -----------------------------

endlocal
