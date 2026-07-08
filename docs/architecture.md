# Architecture

## Runtime Layers

```text
Zephyr-style Firmware Module
  -> telemetry bridge
  -> C++ Embedded Camera Gateway / Device API
  -> Python QA Test Station / Web Dashboard
```

## Firmware Layer

`firmware_zephyr/` is a virtual MCU firmware module. It simulates MCU states, UART telemetry, I2C ambient light sensor registers, SPI image sensor registers, power budgeting and camera control.

The confirmed Zephyr target is `qemu_cortex_m3 / ti_lm3s6965`.

## Telemetry Bridge

For local Windows development, Zephyr prints `$SIMCAM` telemetry packets to stdout. `scripts/windows/run_zephyr.bat` writes this output to:

```text
runtime/zephyr_telemetry.log
```

The C++ Gateway reads the latest valid packet from this log. This replaces a real UART/USB CDC/serial link during local development.

## C++ Gateway

`camera_gateway_cpp/` is the main simulated device API. It exposes HTTP endpoints, parses Zephyr telemetry, manages camera mode state and serves snapshots.

## Python QA Dashboard

`python_test_station/` is an external QA/lab tool. It calls the C++ Gateway via HTTP, analyzes images with OpenCV, displays metrics and creates reports.

