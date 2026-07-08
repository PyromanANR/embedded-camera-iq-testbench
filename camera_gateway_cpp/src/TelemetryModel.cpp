#include "TelemetryModel.hpp"

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
