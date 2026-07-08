# Zephyr Setup

The project uses the confirmed Zephyr target:

```text
qemu_cortex_m3 / ti_lm3s6965
```

Confirmed boot example:

```text
*** Booting Zephyr OS build v4.4.0... ***
Hello World! qemu_cortex_m3/ti_lm3s6965
```

## Check Tools

```powershell
scripts\windows\setup_zephyr_windows.ps1
```

The script checks Python, west, CMake, Ninja, dtc and Zephyr environment variables. It does not reinstall tools that already exist.

## Build

```bat
scripts\windows\build_zephyr.bat
```

Equivalent command inside `firmware_zephyr/`:

```bat
west build -p always -b qemu_cortex_m3 .
```

## Run

```bat
scripts\windows\run_zephyr.bat
```

The run script launches the built Zephyr ELF through Zephyr SDK QEMU and writes simulated UART output to:

```text
runtime/zephyr_telemetry.log
```

The C++ Gateway reads this log as a virtual UART bridge.
