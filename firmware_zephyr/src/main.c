#include "app_state.h"
#include "camera_control.h"
#include "fault_injection.h"
#include "i2c_als_sensor.h"
#include "spi_image_sensor.h"
#include "telemetry_protocol.h"
#include "uart_protocol.h"

#if __has_include(<zephyr/kernel.h>)
#include <zephyr/kernel.h>
#define SIMCAM_SLEEP_MS(ms) k_msleep(ms)
#else
#define SIMCAM_SLEEP_MS(ms) ((void)(ms))
#endif

int main(void) {
    app_state_t app;
    simcam_telemetry_t telemetry;
    char packet[192];

    app_state_init(&app);
    i2c_als_init();
    spi_image_sensor_init();
    camera_control_set_mode(CAMERA_MODE_STOCK);

    app_state_set(&app, SIMCAM_IDLE);

    while (1) {
        app_state_set(&app, SIMCAM_CAPTURE);
        app.frame_counter++;
        app.error_code = fault_injection_error_code();

        camera_control_fill_telemetry(&telemetry, app.frame_counter, app.battery_percent, app.error_code);
        telemetry_format_packet(&telemetry, packet, sizeof(packet));

        app_state_set(&app, SIMCAM_TRANSMIT);
        uart_protocol_send(packet);

        app_state_set(&app, SIMCAM_SLEEP);
        SIMCAM_SLEEP_MS(1000);
    }

    return 0;
}

