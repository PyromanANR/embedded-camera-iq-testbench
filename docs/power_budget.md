# Power Budget

The power model estimates camera device consumption by state.

Constants:

- `MCU_SLEEP_MW = 5`
- `MCU_IDLE_MW = 20`
- `MCU_ACTIVE_MW = 35`
- `IMAGE_SENSOR_MW = 120`
- `WIFI_TX_MW = 300`
- `WLED_MW = 450`
- `IR_LED_MW = 380`
- `UART_DEBUG_MW = 5`

Modes:

- sleep
- idle
- capture
- transmit
- capture with white LED
- capture with IR LED
- low battery
- overheat

Telemetry includes `estimated_power_mw` and `battery_percent` to connect image capture behavior with battery-operated device validation.

