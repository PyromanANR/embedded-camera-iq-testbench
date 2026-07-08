@echo off
setlocal
cd /d "%~dp0..\..\firmware_zephyr"

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

"%WEST_CMD%" build -p always -b qemu_cortex_m3 .
