#include "camera_control.h"

#include "i2c_als_sensor.h"
#include "power_budget.h"

static camera_mode_t active_mode = CAMERA_MODE_STOCK;

void camera_control_set_mode(camera_mode_t mode) {
    active_mode = mode;
}

camera_mode_t camera_control_get_mode(void) {
    return active_mode;
}

void camera_control_fill_telemetry(simcam_telemetry_t *telemetry, uint32_t frame, uint8_t battery_percent, uint8_t error_code) {
    telemetry->frame = frame;
    telemetry->temperature_c = 42.5f;
    telemetry->battery_percent = battery_percent;
    telemetry->lux = i2c_als_read_lux();
    telemetry->exposure_ms = 8.3f;
    telemetry->gain = 1.4f;
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
}

