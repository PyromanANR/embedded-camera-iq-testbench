#include "telemetry_protocol.h"

#include <stdio.h>
#include <string.h>

static const char *led_name(led_mode_t mode) {
    if (mode == LED_WHITE) {
        return "WLED";
    }
    if (mode == LED_IR) {
        return "IR";
    }
    return "OFF";
}

static int tenths_from_float(float value) {
    if (value >= 0.0f) {
        return (int)(value * 10.0f + 0.5f);
    }
    return (int)(value * 10.0f - 0.5f);
}

uint8_t telemetry_crc8(const char *payload) {
    uint8_t crc = 0;
    while (*payload) {
        crc = (uint8_t)(crc + (uint8_t)(*payload));
        payload++;
    }
    return crc;
}

int telemetry_format_packet(const simcam_telemetry_t *t, char *buffer, size_t buffer_size) {
    char body[160];
    const int temp_tenths = tenths_from_float(t->temperature_c);
    const int exposure_tenths = tenths_from_float(t->exposure_ms);
    const int gain_tenths = tenths_from_float(t->gain);
    int body_len = snprintf(body, sizeof(body),
        "FRAME=%lu,TEMP=%d.%u,BAT=%u,LUX=%u,EXP=%d.%u,GAIN=%d.%u,LED=%s,PWR=%u,ERR=%u",
        (unsigned long)t->frame,
        temp_tenths / 10,
        (unsigned int)(temp_tenths < 0 ? -temp_tenths : temp_tenths) % 10,
        t->battery_percent,
        t->lux,
        exposure_tenths / 10,
        (unsigned int)(exposure_tenths < 0 ? -exposure_tenths : exposure_tenths) % 10,
        gain_tenths / 10,
        (unsigned int)(gain_tenths < 0 ? -gain_tenths : gain_tenths) % 10,
        led_name(t->led_mode),
        t->power_mw,
        t->error_code);
    if (body_len < 0 || (size_t)body_len >= sizeof(body)) {
        return -1;
    }
    return snprintf(buffer, buffer_size, "$SIMCAM,%s,CRC=%02X", body, telemetry_crc8(body));
}
