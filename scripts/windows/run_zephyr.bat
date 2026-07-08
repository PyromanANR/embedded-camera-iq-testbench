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

set "ELF=firmware_zephyr\build\zephyr\zephyr.elf"
if not exist "%ELF%" (
  echo Zephyr ELF was not found. Building firmware first...
  call scripts\windows\build_zephyr.bat
  if errorlevel 1 exit /b 1
)

set "QEMU=%USERPROFILE%\zephyr-sdk-1.0.1\hosttools\qemu\qemu-system-arm.exe"
if not exist "%QEMU%" (
  echo qemu-system-arm.exe was not found under %%USERPROFILE%%\zephyr-sdk-1.0.1.
  echo Install the Zephyr SDK or run from a Zephyr environment shell.
  exit /b 1
)

if exist "C:\Program Files\Git\mingw64\bin" set "PATH=C:\Program Files\Git\mingw64\bin;%PATH%"
if exist "C:\tools\msys64\mingw64\bin" set "PATH=C:\tools\msys64\mingw64\bin;%PATH%"

type nul > runtime\zephyr_telemetry.log

echo QEMU is running. Press Ctrl+C to stop firmware simulation.
"%QEMU%" -cpu cortex-m3 -machine lm3s6965evb ^
  -chardev "file,id=serial0,path=%CD%\runtime\zephyr_telemetry.log" ^
  -serial chardev:serial0 ^
  -monitor none ^
  -nographic ^
  -rtc clock=vm ^
  -net none ^
  -kernel "%CD%\%ELF%"
