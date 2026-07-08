cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(simcam_firmware)

target_sources(app PRIVATE
    src/main.c
    src/app_state.c
    src/power_budget.c
    src/telemetry_protocol.c
    src/uart_protocol.c
    src/i2c_als_sensor.c
    src/spi_image_sensor.c
    src/camera_control.c
    src/fault_injection.c
)

target_include_directories(app PRIVATE include)

