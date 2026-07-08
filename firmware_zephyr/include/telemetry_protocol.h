#pragma once

#include <stddef.h>
#include <stdint.h>
#include "power_budget.h"

typedef struct {
    uint32_t frame;
    float temperature_c;
    uint8_t battery_percent;
    uint16_t lux;
    float exposure_ms;
    float gain;
    led_mode_t led_mode;
    uint16_t power_mw;
    uint8_t error_code;
} simcam_telemetry_t;

uint8_t telemetry_crc8(const char *payload);
int telemetry_format_packet(const simcam_telemetry_t *telemetry, char *buffer, size_t buffer_size);

