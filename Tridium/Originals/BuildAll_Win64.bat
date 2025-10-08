@echo off

call %~dp0\assimp\Build_Win64
call %~dp0\glfw\Build_Win64
call %~dp0\yaml-cpp\Build_Win64

echo ALL_COMPLETE