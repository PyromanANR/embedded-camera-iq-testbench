# Zephyr-style Firmware Module

Virtual MCU firmware layer written in C.

It demonstrates:

- MCU states: BOOT, IDLE, CAPTURE, TRANSMIT, SLEEP, ERROR.
- UART telemetry packet formatting.
- I2C ambient light sensor registers.
- SPI image sensor registers.
- Power budgeting.
- Battery, temperature, lux, exposure, gain, LED mode and error telemetry.

Confirmed Zephyr target:

```text
qemu_cortex_m3 / ti_lm3s6965
```

Build from the repository root:

```bat
scripts\windows\build_zephyr.bat
```

Run and write stdout telemetry to `runtime/zephyr_telemetry.log`:

```bat
scripts\windows\run_zephyr.bat
```

This module is a firmware architecture demonstration. The C++ Gateway is the main runtime device API.
