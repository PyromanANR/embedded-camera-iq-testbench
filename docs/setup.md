# Setup

## Build Zephyr Firmware

```bat
scripts\windows\build_zephyr.bat
```

Target:

```text
qemu_cortex_m3
```

## Run Zephyr Telemetry Bridge

```bat
scripts\windows\run_zephyr.bat
```

Output:

```text
runtime/zephyr_telemetry.log
```

## Build C++ Gateway

```bat
scripts\windows\build_cpp_gateway.bat
```

## Run C++ Gateway

```bat
scripts\windows\run_cpp_gateway.bat
```

URL:

```text
http://127.0.0.1:8081
```

## Python Dashboard

```bat
scripts\windows\setup_python_env.bat
scripts\windows\run_python_dashboard.bat
```

Open:

```text
http://127.0.0.1:8000
```

## Add Images

```text
data/frames/stock/
```

