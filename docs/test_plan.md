# Test Plan

## Scope

Validate a virtual embedded camera through a C++ device API and Python QA dashboard.

## In Scope

- C++ Gateway health, telemetry, snapshot and control endpoints.
- Python dashboard connection status, Dev Controls and reports.
- Image quality metrics: sharpness, noise, exposure, contrast and color cast.
- Missing image fallback behavior.
- Zephyr-style firmware source structure and build instructions.

## Out of Scope

- Physical camera modules.
- Real Wi-Fi throughput.
- RTSP/MJPEG streaming.
- Production ISP tuning.
- ML model training.

## Environment

- Windows with Visual Studio 2022 and CMake.
- Python 3.12.
- C++ Gateway on `127.0.0.1:8081`.
- Python dashboard on `127.0.0.1:8000`.
- Optional WSL Ubuntu for Zephyr.

## Test Data

Add your original clean camera images to:

```text
data/frames/stock/
```

Fault folders are optional. Empty folders trigger generated placeholder/fault images.

## Pass / Fail Criteria

- PASS: no major issue detected.
- WARNING: marginal metric or minor issue.
- FAIL: severe blur, exposure issue, high noise, low contrast or strong color cast.

