#include "PowerModel.hpp"

int PowerModel::estimateMilliwatts(const std::string& powerState, const std::string& ledMode) const {
    constexpr int MCU_SLEEP_MW = 5;
    constexpr int MCU_IDLE_MW = 20;
    constexpr int MCU_ACTIVE_MW = 35;
    constexpr int IMAGE_SENSOR_MW = 120;
    constexpr int WIFI_TX_MW = 300;
    constexpr int WLED_MW = 450;
    constexpr int IR_LED_MW = 380;
    constexpr int UART_DEBUG_MW = 5;

    if (powerState == "sleep") {
        return MCU_SLEEP_MW;
    }
    if (powerState == "idle") {
        return MCU_IDLE_MW + UART_DEBUG_MW;
    }
    if (powerState == "transmit") {
        return MCU_ACTIVE_MW + IMAGE_SENSOR_MW + WIFI_TX_MW + UART_DEBUG_MW;
    }

    int total = MCU_ACTIVE_MW + IMAGE_SENSOR_MW + UART_DEBUG_MW;
    if (ledMode == "WLED") {
        total += WLED_MW;
    } else if (ledMode == "IR") {
        total += IR_LED_MW;
    }
    return total;
}

