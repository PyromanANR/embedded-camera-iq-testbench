#pragma once

#include <mutex>
#include <string>
#include <chrono>

#include "TelemetryModel.hpp"
#include "TelemetryReader.hpp"

class CameraDevice {
public:
    std::string healthJson();
    std::string telemetryJson();
    std::string setMode(const std::string& mode);
    std::string reset();
    std::string activeMode() const;

private:
    mutable std::mutex mutex_;
    std::string activeMode_ = "stock";
    int frameId_ = 0;
    std::chrono::steady_clock::time_point bootTime_ = std::chrono::steady_clock::now();
    TelemetryModel telemetryModel_;
    TelemetryReader telemetryReader_;
};
