@echo off

set PLATFORM=x64
set CONFIG=Release
set SOLUTION=%~dp0\intermediate\build_win\GLFW.sln
set PROJECT=GLFW
set CMAKE_PATH=%~dp0\..\..\..\Dependencies\Cmake\bin\cmake.exe
set PROJECT_SOURCE_PATH=%~dp0\glfw
set PROJECT_OUTPUT_PATH=%~dp0\intermediate\build_win
set BINARY_SOURCE_PATH=%~dp0\intermediate\build_win\src\%CONFIG%
set BINARY_OUTPUT_PATH=%~dp0\..\..\Dependencies\glfw\bin\windows\%CONFIG%

rd /s /q %PROJECT_OUTPUT_PATH%
mkdir %PROJECT_OUTPUT_PATH%

%CMAKE_PATH% -S %PROJECT_SOURCE_PATH% -B %PROJECT_OUTPUT_PATH% ^
    -DBUILD_SHARED_LIBS=OFF ^
    -DGLFW_BUILD_EXAMPLES=OFF ^
    -DGLFW_BUILD_TESTS=OFF ^
    -DGLFW_BUILD_DOCS=OFF ^
    -DGLFW_INSTALL=OFF ^
    -DCMAKE_MSVC_RUNTIME_LIBRARY=MultiThreaded

devenv %SOLUTION% /build "%CONFIG%|%PLATFORM%" /project %PROJECT%

rd /s /q %BINARY_OUTPUT_PATH%
mkdir %BINARY_OUTPUT_PATH%
xcopy /s %BINARY_SOURCE_PATH% %BINARY_OUTPUT_PATH%