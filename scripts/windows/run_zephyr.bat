@echo off
setlocal
cd /d "%~dp0..\.."

if not exist runtime mkdir runtime
echo Starting Zephyr qemu_cortex_m3 telemetry bridge...
echo Output log: runtime\zephyr_telemetry.log

set "WEST_CMD=west"
where west >nul 2>nul
if errorlevel 1 (
  if exist "%USERPROFILE%\zephyrproject\.venv\Scripts\west.exe" (
    set "WEST_CMD=%USERPROFILE%\zephyrproject\.venv\Scripts\west.exe"
  ) else (
    echo west was not found on PATH or in %%USERPROFILE%%\zephyrproject\.venv\Scripts.
    echo Run scripts\windows\setup_zephyr_windows.ps1 or open a Zephyr environment shell.
    exit /b 1
  )
)

if not defined ZEPHYR_BASE (
  if exist "%USERPROFILE%\zephyrproject\zephyr" (
    set "ZEPHYR_BASE=%USERPROFILE%\zephyrproject\zephyr"
  )
)

set "SIMCAM_WEST=%WEST_CMD%"

powershell -NoProfile -ExecutionPolicy Bypass -Command ^
  "$ErrorActionPreference='Stop';" ^
  "$root=(Resolve-Path '.').Path;" ^
  "$log=Join-Path $root 'runtime\zephyr_telemetry.log';" ^
  "$west=$env:SIMCAM_WEST;" ^
  "Set-Content -Path $log -Value '';" ^
  "Push-Location (Join-Path $root 'firmware_zephyr');" ^
  "& $west build -t run 2>&1 | Tee-Object -FilePath $log -Append;" ^
  "Pop-Location"
