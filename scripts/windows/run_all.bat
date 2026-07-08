@echo off
setlocal
cd /d "%~dp0..\.."

if not exist "camera_gateway_cpp\build\Release\simcam_gateway.exe" (
  call scripts\windows\build_cpp_gateway.bat
  if errorlevel 1 exit /b 1
)

start "SIMCAM Zephyr Firmware" cmd /k scripts\windows\run_zephyr.bat
timeout /t 2 >nul
start "SIMCAM C++ Gateway" cmd /k scripts\windows\run_cpp_gateway.bat
timeout /t 2 >nul
start "SIMCAM Python Dashboard" cmd /k scripts\windows\run_python_dashboard.bat

echo.
echo Open http://127.0.0.1:8000
