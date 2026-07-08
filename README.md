# Embedded Camera IQ Testbench

A virtual embedded smart camera validation system with a Zephyr-style firmware layer, C++ camera gateway, and Python image quality QA dashboard.

## Overview

This repository models a realistic camera validation workflow without physical camera hardware. Zephyr firmware runs in QEMU, emits UART-style telemetry, the C++ Gateway reads that telemetry through a local bridge and exposes an IP-camera-like HTTP API, and the Python dashboard acts as an external QA/lab station.

Confirmed Zephyr target:

```text
qemu_cortex_m3 / ti_lm3s6965
```

Example confirmed output:

```text
*** Booting Zephyr OS build v4.4.0... ***
Hello World! qemu_cortex_m3/ti_lm3s6965
```

## Why This Project Exists

The project is built for Embedded Intern/Trainee, Image Quality Engineer Intern, AI Hardware QA, and Computer Vision / Camera QA roles. It demonstrates embedded C, C++, CMake, Zephyr, telemetry, simulated UART/I2C/SPI concepts, HTTP/JSON networking, power budgeting, Python, FastAPI, OpenCV image quality metrics, Dev Controls, reports, and QA documentation.

## Architecture

```text
Zephyr-style Firmware Module
  -> telemetry bridge
  -> C++ Embedded Camera Gateway / Device API
  -> HTTP API
  -> Python QA Test Station / Web Dashboard
```

Runtime:

```text
Zephyr firmware in QEMU
  -> stdout / runtime/zephyr_telemetry.log
  -> C++ Gateway parses latest $SIMCAM packet
  -> Python dashboard requests /health, /telemetry and /snapshot.jpg
```

In real hardware this bridge would be UART / USB CDC / serial instead of a log file.

## Features

- Zephyr firmware module with MCU states, simulated I2C ALS, simulated SPI image sensor, UART-style telemetry and power budget.
- C++17 Camera Gateway on `127.0.0.1:8081`.
- Telemetry bridge from `runtime/zephyr_telemetry.log`.
- Offline detection when firmware telemetry is missing or stale.
- Snapshot endpoint with stock images and generated degradations.
- Python FastAPI dashboard on `127.0.0.1:8000`.
- Dev Controls for camera fault modes.
- OpenCV IQ metrics and PASS / WARNING / FAIL verdict.
- JSON and HTML reports.

## Tech Stack

- C / Zephyr RTOS / QEMU
- C++17 / CMake / Visual Studio 2022
- Python / FastAPI / Jinja2
- OpenCV / NumPy / Pillow
- Pytest

## Screenshots



## Demo Video



## Quick Start

```bat
scripts\windows\build_zephyr.bat
scripts\windows\run_zephyr.bat
```

In a second terminal:

```bat
scripts\windows\build_cpp_gateway.bat
scripts\windows\run_cpp_gateway.bat
```

In a third terminal:

```bat
scripts\windows\setup_python_env.bat
scripts\windows\run_python_dashboard.bat
```

Open:

```text
http://127.0.0.1:8000
```

Or run all services:

```bat
scripts\windows\run_all.bat
```

## Add Your Own Images

Add original clean camera images to:

```text
data/frames/stock/
```

Optional fault samples:

```text
data/frames/blurry/
data/frames/noisy/
data/frames/overexposed/
data/frames/underexposed/
data/frames/low_contrast/
data/frames/color_cast/
data/frames/jpeg_artifacts/
data/frames/dead_pixels/
data/frames/low_light/
```

If a fault folder is empty, the C++ Gateway applies the degradation to a stock image. If stock is empty, it generates a placeholder.

## API Endpoints

C++ Gateway base URL:

```text
http://127.0.0.1:8081
```

- `GET /health`
- `GET /telemetry`
- `GET /snapshot.jpg`
- `POST /control/mode`
- `POST /control/reset`
- `GET /dev/photo-paths`

Mode command:

```json
{"mode": "blurry"}
```

Supported modes: `stock`, `blurry`, `noisy`, `overexposed`, `underexposed`, `low_contrast`, `color_cast`, `jpeg_artifacts`, `dead_pixels`, `low_light`, `random_fault`.

## Zephyr Firmware

Build target:

```text
qemu_cortex_m3
```

Build:

```bat
scripts\windows\build_zephyr.bat
```

Run the firmware simulation and write the UART-style serial output to the telemetry bridge log:

```bat
scripts\windows\run_zephyr.bat
```

On Windows, the run script launches the built Zephyr ELF through the Zephyr SDK QEMU and maps the simulated UART to `runtime/zephyr_telemetry.log`. This avoids fragile stdout piping while preserving the same local bridge concept.

Telemetry packet:

```text
$SIMCAM,FRAME=128,TEMP=42.5,BAT=87,LUX=320,EXP=8.3,GAIN=1.4,LED=IR,PWR=184,ERR=0,CRC=7A
```

## C++ Camera Gateway

The C++ Gateway is the main simulated device API. It reads the latest valid `$SIMCAM` packet from:

```text
runtime/zephyr_telemetry.log
```

If the log is missing, invalid, or older than three seconds, `/telemetry` returns `firmware_offline` and a warning instead of fake online telemetry.

## Python QA Dashboard

The dashboard is an external QA/lab client. It does not simulate the camera and does not read Zephyr directly. It only calls the C++ Gateway.

Pages:

- Dashboard
- Dev Controls
- Reports
- Setup

## Reports

Reports are saved to:

```text
python_test_station/reports/
```

Each report includes timestamp, active mode, telemetry, metrics, verdict, issues and snapshot reference.

## Project Structure

```text
firmware_zephyr/       C / Zephyr virtual MCU firmware
camera_gateway_cpp/    C++ Camera Gateway and HTTP API
python_test_station/   Python QA dashboard and OpenCV metrics
data/frames/           User images and optional fault samples
runtime/               Local telemetry bridge files
docs/                  GitHub docs and interview prep document
scripts/windows/       Windows build/run scripts
```

## Testing

```bat
scripts\windows\run_tests.bat
scripts\windows\build_cpp_gateway.bat
```

## Roadmap / TODO

- Real STM32/Nordic board port.
- Real UART/USB CDC bridge.
- ESP32-CAM / Raspberry Pi camera support.
- MJPEG/RTSP stream.
- ONNX model for defect classification.
- GitHub Actions CI.
- Docker support.
- Calibration chart support.
- More objective image quality metrics.

## License

MIT License. Copyright (c) 2026 Nikita Avdeev.
