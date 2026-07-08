#pragma once

#include <string>

struct Telemetry {
    int frameId = 0;
    double temperatureC = 42.5;
    int batteryPercent = 87;
    int lux = 320;
    double exposureMs = 8.3;
    double sensorGain = 1.4;
    std::string ledMode = "OFF";
    std::string powerState = "capture";
    int estimatedPowerMw = 184;
    int droppedFrames = 0;
    int errorCode = 0;
    std::string connectionStatus = "firmware_online";
    std::string warning;
    std::string activeMode = "stock";
};

class TelemetryModel {
public:
    Telemetry next(const std::string& mode, int frameId);
    Telemetry applyModeOverlay(Telemetry telemetry, const std::string& mode) const;
    std::string toJson(const Telemetry& telemetry) const;
    std::string offlineJson(const std::string& activeMode, const std::string& warning) const;
};
