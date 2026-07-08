#pragma once

#include <stdint.h>
#include "app_state.h"

#define MCU_SLEEP_MW 5
#define MCU_IDLE_MW 20
#define MCU_ACTIVE_MW 35
#define IMAGE_SENSOR_MW 120
#define WIFI_TX_MW 300
#define WLED_MW 450
#define IR_LED_MW 380
#define UART_DEBUG_MW 5

typedef enum {
    LED_OFF = 0,
    LED_WHITE,
    LED_IR
} led_mode_t;

uint16_t power_budget_estimate_mw(simcam_state_t state, led_mode_t led_mode);

