#include "power_budget.h"

uint16_t power_budget_estimate_mw(simcam_state_t state, led_mode_t led_mode) {
    uint16_t total = 0;
    if (state == SIMCAM_SLEEP) {
        return MCU_SLEEP_MW;
    }
    if (state == SIMCAM_IDLE) {
        return MCU_IDLE_MW + UART_DEBUG_MW;
    }
    total = MCU_ACTIVE_MW + IMAGE_SENSOR_MW + UART_DEBUG_MW;
    if (state == SIMCAM_TRANSMIT) {
        total += WIFI_TX_MW;
    }
    if (led_mode == LED_WHITE) {
        total += WLED_MW;
    } else if (led_mode == LED_IR) {
        total += IR_LED_MW;
    }
    return total;
}

