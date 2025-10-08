@echo off

set PLATFORM=x64
set CONFIG=Debug
set SOLUTION=%~dp0intermediate\build_win\YAML_CPP.sln
set CMAKE_PATH=%~dp0..\..\..\Dependencies\Cmake\bin\cmake.exe
set PROJECT_SOURCE_PATH=%~dp0yaml-cpp
set PROJECT_OUTPUT_PATH=%~dp0intermediate\build_win
set BINARY_SOURCE_PATH=%PROJECT_OUTPUT_PATH%\%CONFIG%
set BINARY_OUTPUT_PATH=%~dp0..\..\Dependencies\yaml-cpp\bin\windows\%CONFIG%

rem Delete and recreate build folder
rd /s /q "%PROJECT_OUTPUT_PATH%"
mkdir "%PROJECT_OUTPUT_PATH%"

rem Run CMake
"%CMAKE_PATH%" -S "%PROJECT_SOURCE_PATH%" -B "%PROJECT_OUTPUT_PATH%" ^
  -DYAML_BUILD_SHARED_LIBS=OFF ^
  -DYAML_CPP_BUILD_CONTRIB=ON ^
  -DYAML_CPP_BUILD_TOOLS=OFF ^
  -DYAML_CPP_INSTALL=OFF ^
  -DYAML_CPP_DISABLE_UNINSTALL=ON ^
  -DYAML_ENABLE_PIC=ON ^
  -DCMAKE_MSVC_RUNTIME_LIBRARY=MultiThreadedDebug   # for Debug

rem Build the solution (all projects)
devenv "%SOLUTION%" /build "%CONFIG%|%PLATFORM%"

rem Copy binaries
rd /s /q "%BINARY_OUTPUT_PATH%"
mkdir "%BINARY_OUTPUT_PATH%"
xcopy /s /i /y "%BINARY_SOURCE_PATH%" "%BINARY_OUTPUT_PATH%"