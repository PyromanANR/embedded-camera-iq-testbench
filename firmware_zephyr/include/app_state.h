#pragma once

#include <stdint.h>

typedef enum {
    SIMCAM_BOOT = 0,
    SIMCAM_IDLE,
    SIMCAM_CAPTURE,
    SIMCAM_TRANSMIT,
    SIMCAM_SLEEP,
    SIMCAM_ERROR
} simcam_state_t;

typedef struct {
    simcam_state_t state;
    uint32_t frame_counter;
    uint8_t battery_percent;
    uint8_t error_code;
} app_state_t;

void app_state_init(app_state_t *app);
void app_state_set(app_state_t *app, simcam_state_t state);
const char *app_state_name(simcam_state_t state);

