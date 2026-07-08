@echo off
setlocal
cd /d "%~dp0..\.."

if exist "python_test_station\.venv\Scripts\activate.bat" (
  call "python_test_station\.venv\Scripts\activate.bat"
)

python -m pytest python_test_station\tests
