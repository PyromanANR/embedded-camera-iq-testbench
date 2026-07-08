# Troubleshooting

## C++ Gateway Is Offline

Run:

```bat
scripts\windows\run_cpp_gateway.bat
```

The dashboard expects the gateway at:

```text
http://127.0.0.1:8081
```

## Port 8081 Is Busy

Stop the previous gateway process or change the gateway port in source/configuration.

## Port 8000 Is Busy

Stop the previous dashboard process or run Uvicorn on another port.

## No Images in Stock Folder

This is allowed. The C++ Gateway returns a generated placeholder.

Add your original clean camera images to:

```text
data/frames/stock/
```

## OpenCV Install Error

Run:

```bat
scripts\windows\setup_python_env.bat
```

## PyCharm Interpreter Issue

Make sure PyCharm uses the same interpreter or virtual environment where `python_test_station/requirements.txt` was installed.

## Zephyr Not Installed or Not on PATH

Check the current Windows environment:

```powershell
scripts\windows\setup_zephyr_windows.ps1
```

Then build the confirmed QEMU target:

```bat
scripts\windows\build_zephyr.bat
```
