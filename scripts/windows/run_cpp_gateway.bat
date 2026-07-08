@echo off
setlocal
cd /d "%~dp0..\.."

if exist "camera_gateway_cpp\build\Release\simcam_gateway.exe" (
  camera_gateway_cpp\build\Release\simcam_gateway.exe
  exit /b %errorlevel%
)

if exist "camera_gateway_cpp\build\Debug\simcam_gateway.exe" (
  echo Release executable was not found; running Debug build.
  camera_gateway_cpp\build\Debug\simcam_gateway.exe
  exit /b %errorlevel%
)

if exist "camera_gateway_cpp\build\simcam_gateway.exe" (
  camera_gateway_cpp\build\simcam_gateway.exe
  exit /b %errorlevel%
)

echo C++ Gateway executable was not found. Run scripts\windows\build_cpp_gateway.bat first.
exit /b 1
