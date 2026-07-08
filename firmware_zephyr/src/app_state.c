#include "app_state.h"

void app_state_init(app_state_t *app) {
    app->state = SIMCAM_BOOT;
    app->frame_counter = 0;
    app->battery_percent = 87;
    app->error_code = 0;
}

void app_state_set(app_state_t *app, simcam_state_t state) {
    app->state = state;
}

const char *app_state_name(simcam_state_t state) {
    switch (state) {
    case SIMCAM_BOOT: return "BOOT";
    case SIMCAM_IDLE: return "IDLE";
    case SIMCAM_CAPTURE: return "CAPTURE";
    case SIMCAM_TRANSMIT: return "TRANSMIT";
    case SIMCAM_SLEEP: return "SLEEP";
    case SIMCAM_ERROR: return "ERROR";
    default: return "UNKNOWN";
    }
}

