@echo off
setlocal
cd /d "%~dp0..\.."

if not exist "python_test_station\.venv\Scripts\python.exe" (
  python -m venv python_test_station\.venv
)

call "python_test_station\.venv\Scripts\activate.bat"
python -m pip install --upgrade pip
python -m pip install -r python_test_station\requirements.txt

echo.
echo Python environment is ready.
