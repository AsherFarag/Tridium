@echo off

set PLATFORM=x64
set CONFIG=Release
set SOLUTION=%~dp0\intermediate\build_win\Assimp.sln
set PROJECT=assimp
set CMAKE_PATH=%~dp0\..\..\..\Dependencies\Cmake\bin\cmake.exe
set PROJECT_SOURCE_PATH=%~dp0\assimp
set PROJECT_OUTPUT_PATH=%~dp0\intermediate\build_win
set BINARY_SOURCE_PATH=%~dp0\intermediate\build_win\lib\%CONFIG%
set BINARY_OUTPUT_PATH=%~dp0\..\..\Dependencies\assimp\bin\windows\Debug

rd /s /q %PROJECT_OUTPUT_PATH%
mkdir %PROJECT_OUTPUT_PATH%

%CMAKE_PATH% -S %PROJECT_SOURCE_PATH% -B %PROJECT_OUTPUT_PATH% ^
	-DBUILD_SHARED_LIBS=OFF ^
	-DASSIMP_NO_EXPORT=OFF ^
	-DASSIMP_INSTALL=OFF ^
	-DUSE_STATIC_CRT=ON ^
	-DASSIMP_BUILD_ZLIB=ON

devenv %SOLUTION% /build "%CONFIG%|%PLATFORM%" /project %PROJECT%

rd /s /q %BINARY_OUTPUT_PATH%
mkdir %BINARY_OUTPUT_PATH%
xcopy /s %BINARY_SOURCE_PATH% %BINARY_OUTPUT_PATH%