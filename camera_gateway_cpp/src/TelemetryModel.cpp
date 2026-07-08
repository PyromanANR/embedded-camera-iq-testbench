#include "TelemetryModel.hpp"

#include <algorithm>
#include <sstream>

#include "PowerModel.hpp"

Telemetry TelemetryModel::next(const std::string& mode, int frameId) {
    PowerModel powerModel;
    Telemetry telemetry;
    telemetry.frameId = frameId;
    telemetry.activeMode = mode;

    if (mode == "low_light") {
        telemetry.lux = 18;
        telemetry.exposureMs = 33.0;
        telemetry.sensorGain = 6.0;
        telemetry.ledMode = "IR";
    } else if (mode == "overexposed") {
        telemetry.lux = 1200;
        telemetry.exposureMs = 18.0;
    } else if (mode == "underexposed") {
        telemetry.lux = 45;
        telemetry.exposureMs = 3.0;
    } else if (mode == "jpeg_artifacts") {
        telemetry.powerState = "transmit";
    } else if (mode == "noisy") {
        telemetry.sensorGain = 7.5;
    } else if (mode == "blurry") {
        telemetry.exposureMs = 40.0;
    } else if (mode == "dead_pixels") {
        telemetry.droppedFrames = 2;
    }

    telemetry.errorCode = mode == "stock" ? 0 : 10;
    telemetry.estimatedPowerMw = powerModel.estimateMilliwatts(telemetry.powerState, telemetry.ledMode);
    return telemetry;
}

Telemetry TelemetryModel::applyModeOverlay(Telemetry telemetry, const std::string& mode) const {
    PowerModel powerModel;
    const int phase = (telemetry.frameId / 2) % 5;
    const int jitter = phase - 2;

    telemetry.temperatureC += static_cast<double>(jitter) * 0.1;
    telemetry.lux = std::max(0, telemetry.lux + jitter * 7);
    telemetry.exposureMs = std::max(0.1, telemetry.exposureMs + static_cast<double>(phase) * 0.1);
    telemetry.sensorGain = std::max(1.0, telemetry.sensorGain + static_cast<double>(phase) * 0.05);

    if (mode == "low_light") {
        telemetry.lux = std::max(1, 16 + jitter * 2);
        telemetry.exposureMs = 32.0 + static_cast<double>(phase) * 0.4;
        telemetry.sensorGain = 5.8 + static_cast<double>(phase) * 0.2;
        telemetry.ledMode = "IR";
        telemetry.powerState = "capture_ir";
    } else if (mode == "noisy") {
        telemetry.sensorGain = 6.8 + static_cast<double>(phase) * 0.25;
    } else if (mode == "overexposed") {
        telemetry.lux = 1120 + phase * 25;
        telemetry.exposureMs = 17.0 + static_cast<double>(phase) * 0.2;
    } else if (mode == "underexposed") {
        telemetry.lux = std::max(5, 48 + jitter * 3);
        telemetry.exposureMs = 2.8 + static_cast<double>(phase) * 0.1;
    } else if (mode == "blurry") {
        telemetry.exposureMs = 38.0 + static_cast<double>(phase) * 0.6;
    } else if (mode == "jpeg_artifacts") {
        telemetry.powerState = "transmit";
    } else if (mode == "dead_pixels") {
        telemetry.droppedFrames = phase % 3;
    }

    const std::string powerStateForModel = telemetry.powerState == "capture_ir" ? "capture" : telemetry.powerState;
    telemetry.estimatedPowerMw = powerModel.estimateMilliwatts(powerStateForModel, telemetry.ledMode) + phase * 4;
    telemetry.errorCode = mode == "stock" ? 0 : 10;
    telemetry.activeMode = mode;
    return telemetry;
}

std::string TelemetryModel::toJson(const Telemetry& t) const {
    std::ostringstream json;
    json << "{"
         << "\"device_id\":\"SIMCAM-001\","
         << "\"firmware_version\":\"0.1.0\","
         << "\"frame_id\":" << t.frameId << ","
         << "\"temperature_c\":" << t.temperatureC << ","
         << "\"battery_percent\":" << t.batteryPercent << ","
         << "\"lux\":" << t.lux << ","
         << "\"exposure_ms\":" << t.exposureMs << ","
         << "\"sensor_gain\":" << t.sensorGain << ","
         << "\"led_mode\":\"" << t.ledMode << "\","
         << "\"power_state\":\"" << t.powerState << "\","
         << "\"estimated_power_mw\":" << t.estimatedPowerMw << ","
         << "\"dropped_frames\":" << t.droppedFrames << ","
         << "\"connection_status\":\"" << t.connectionStatus << "\","
         << "\"error_code\":" << t.errorCode << ","
         << "\"active_mode\":\"" << t.activeMode << "\""
         << "}";
    return json.str();
}

std::string TelemetryModel::offlineJson(const std::string& activeMode, const std::string& warning) const {
    std::ostringstream json;
    json << "{"
         << "\"device_id\":\"SIMCAM-001\","
         << "\"firmware_version\":\"0.1.0\","
         << "\"connection_status\":\"firmware_offline\","
         << "\"warning\":\"" << warning << "\","
         << "\"active_mode\":\"" << activeMode << "\""
         << "}";
    return json.str();
}
