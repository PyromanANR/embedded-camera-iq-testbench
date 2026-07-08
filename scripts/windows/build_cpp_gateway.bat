@echo off
setlocal
cd /d "%~dp0..\.."

where cmake >nul 2>nul
if errorlevel 1 (
  echo CMake was not found on PATH.
  exit /b 1
)

cmake -S camera_gateway_cpp -B camera_gateway_cpp\build -G "Visual Studio 17 2022" -A x64
if errorlevel 1 (
  echo Visual Studio generator failed. Trying default CMake generator...
  cmake -S camera_gateway_cpp -B camera_gateway_cpp\build
)
if errorlevel 1 exit /b 1

cmake --build camera_gateway_cpp\build --config Release
