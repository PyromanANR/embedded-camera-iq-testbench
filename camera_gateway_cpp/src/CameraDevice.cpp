#include "CameraDevice.hpp"

#include <sstream>
#include <stdexcept>
#include <random>
#include <vector>

namespace {
bool isValidMode(const std::string& mode) {
    static const std::vector<std::string> modes = {
        "stock", "blurry", "noisy", "overexposed", "underexposed",
        "low_contrast", "color_cast", "jpeg_artifacts", "dead_pixels", "low_light"
    };
    for (const auto& item : modes) {
        if (item == mode) {
            return true;
        }
    }
    return false;
}
}

std::string CameraDevice::healthJson() {
    std::lock_guard<std::mutex> lock(mutex_);
    const auto uptimeMs = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - bootTime_).count();
    const auto telemetry = telemetryReader_.readLatest();
    const std::string firmwareStatus = telemetry.valid && telemetry.fresh ? "firmware_online" : "firmware_offline";
    std::ostringstream json;
    json << "{"
         << "\"status\":\"ok\","
         << "\"device_id\":\"SIMCAM-001\","
         << "\"firmware_version\":\"0.1.0\","
         << "\"uptime_ms\":" << uptimeMs << ","
         << "\"active_mode\":\"" << activeMode_ << "\","
         << "\"telemetry_source\":\"zephyr_bridge\","
         << "\"firmware_status\":\"" << firmwareStatus << "\""
         << "}";
    return json.str();
}

std::string CameraDevice::telemetryJson() {
    std::lock_guard<std::mutex> lock(mutex_);
    const auto result = telemetryReader_.readLatest();
    if (!result.valid || !result.fresh || !result.telemetry.has_value()) {
        return telemetryModel_.offlineJson(activeMode_, result.warning.empty()
            ? "No fresh Zephyr telemetry received. Start firmware simulation."
            : result.warning);
    }
    auto telemetry = *result.telemetry;
    telemetry.activeMode = activeMode_;
    telemetry.connectionStatus = "firmware_online";
    return telemetryModel_.toJson(telemetry);
}

std::string CameraDevice::setMode(const std::string& mode) {
    std::lock_guard<std::mutex> lock(mutex_);
    std::string normalized = mode == "blur" ? "blurry" : mode;
    if (normalized == "random_fault") {
        static const std::vector<std::string> faultModes = {
            "blurry", "noisy", "overexposed", "underexposed", "low_contrast",
            "color_cast", "jpeg_artifacts", "dead_pixels", "low_light"
        };
        static std::mt19937 rng(std::random_device{}());
        std::uniform_int_distribution<std::size_t> distribution(0, faultModes.size() - 1);
        normalized = faultModes[distribution(rng)];
    }
    if (!isValidMode(normalized)) {
        throw std::invalid_argument("Unsupported mode");
    }
    activeMode_ = normalized;
    return "{\"ok\":true,\"active_mode\":\"" + activeMode_ + "\"}";
}

std::string CameraDevice::reset() {
    std::lock_guard<std::mutex> lock(mutex_);
    activeMode_ = "stock";
    frameId_ = 0;
    bootTime_ = std::chrono::steady_clock::now();
    return "{\"ok\":true,\"active_mode\":\"stock\"}";
}

std::string CameraDevice::activeMode() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return activeMode_;
}
