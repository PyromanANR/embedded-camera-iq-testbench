@echo off
setlocal
cd /d "%~dp0..\.."

if exist "python_test_station\.venv\Scripts\activate.bat" (
  call "python_test_station\.venv\Scripts\activate.bat"
)

cd /d "%~dp0..\..\python_test_station"
python -m uvicorn app.main:app --host 127.0.0.1 --port 8000 --reload
