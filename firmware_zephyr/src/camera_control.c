#include "camera_control.h"

#include "i2c_als_sensor.h"
#include "power_budget.h"

static camera_mode_t active_mode = CAMERA_MODE_STOCK;

static int16_t telemetry_jitter(uint32_t frame, uint8_t period, int16_t amplitude) {
    int16_t phase = (int16_t)((frame / 2U) % period);
    int16_t center = (int16_t)(period / 2U);
    return (int16_t)((phase - center) * amplitude);
}

void camera_control_set_mode(camera_mode_t mode) {
    active_mode = mode;
}

camera_mode_t camera_control_get_mode(void) {
    return active_mode;
}

void camera_control_fill_telemetry(simcam_telemetry_t *telemetry, uint32_t frame, uint8_t battery_percent, uint8_t error_code) {
    int16_t lux_jitter = telemetry_jitter(frame, 7, 6);
    int16_t temp_jitter = telemetry_jitter(frame, 5, 1);
    int16_t exposure_jitter = telemetry_jitter(frame, 5, 1);
    int16_t gain_jitter = telemetry_jitter(frame, 5, 1);

    telemetry->frame = frame;
    telemetry->temperature_c = 42.5f + ((float)temp_jitter * 0.1f);
    telemetry->battery_percent = battery_percent;
    telemetry->lux = (uint16_t)((int16_t)i2c_als_read_lux() + lux_jitter);
    telemetry->exposure_ms = 8.3f + ((float)exposure_jitter * 0.1f);
    telemetry->gain = 1.4f + ((float)gain_jitter * 0.05f);
    telemetry->led_mode = LED_OFF;
    telemetry->error_code = error_code;

    if (active_mode == CAMERA_MODE_LOW_LIGHT) {
        telemetry->lux = 18;
        telemetry->exposure_ms = 33.0f;
        telemetry->gain = 6.0f;
        telemetry->led_mode = LED_IR;
    } else if (active_mode == CAMERA_MODE_NOISY) {
        telemetry->gain = 7.5f;
    } else if (active_mode == CAMERA_MODE_OVEREXPOSED) {
        telemetry->lux = 1200;
        telemetry->exposure_ms = 18.0f;
    } else if (active_mode == CAMERA_MODE_UNDEREXPOSED) {
        telemetry->lux = 45;
        telemetry->exposure_ms = 3.0f;
    }

    telemetry->power_mw = power_budget_estimate_mw(SIMCAM_CAPTURE, telemetry->led_mode);
    telemetry->power_mw = (uint16_t)(telemetry->power_mw + (uint16_t)((frame / 2U) % 5U) * 4U);
}
