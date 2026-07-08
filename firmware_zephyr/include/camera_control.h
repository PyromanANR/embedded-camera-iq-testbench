#pragma once

#include "telemetry_protocol.h"

typedef enum {
    CAMERA_MODE_STOCK = 0,
    CAMERA_MODE_BLURRY,
    CAMERA_MODE_NOISY,
    CAMERA_MODE_OVEREXPOSED,
    CAMERA_MODE_UNDEREXPOSED,
    CAMERA_MODE_LOW_LIGHT
} camera_mode_t;

void camera_control_set_mode(camera_mode_t mode);
camera_mode_t camera_control_get_mode(void);
void camera_control_fill_telemetry(simcam_telemetry_t *telemetry, uint32_t frame, uint8_t battery_percent, uint8_t error_code);

