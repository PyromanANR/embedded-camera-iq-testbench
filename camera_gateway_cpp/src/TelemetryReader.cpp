#include "TelemetryReader.hpp"

#include <fstream>
#include <utility>

#include "TelemetryParser.hpp"

TelemetryReader::TelemetryReader(std::filesystem::path logPath) {
    if (!logPath.empty()) {
        logPath_ = std::move(logPath);
        return;
    }
    const auto root = locateProjectRoot(std::filesystem::current_path());
    logPath_ = root / "runtime" / "zephyr_telemetry.log";
}

TelemetryReadResult TelemetryReader::readLatest() const {
    TelemetryReadResult result;
    if (!std::filesystem::exists(logPath_)) {
        result.warning = "No fresh Zephyr telemetry received. Start firmware simulation.";
        return result;
    }

    result.fresh = isFresh(logPath_, std::chrono::seconds(3));
    const auto line = latestPacketLine(logPath_);
    if (!line.has_value()) {
        result.warning = "No valid $SIMCAM packet found in telemetry log.";
        return result;
    }

    result.telemetry = TelemetryParser::parseSimcamPacket(*line);
    result.valid = result.telemetry.has_value();
    if (!result.valid) {
        result.warning = "Latest $SIMCAM packet could not be parsed.";
        return result;
    }
    if (!result.fresh) {
        result.warning = "No fresh Zephyr telemetry received. Start firmware simulation.";
        return result;
    }
    return result;
}

std::filesystem::path TelemetryReader::logPath() const {
    return logPath_;
}

std::filesystem::path TelemetryReader::locateProjectRoot(std::filesystem::path start) {
    for (auto candidate = std::move(start); !candidate.empty(); candidate = candidate.parent_path()) {
        if (std::filesystem::exists(candidate / "runtime") &&
            std::filesystem::exists(candidate / "camera_gateway_cpp")) {
            return candidate;
        }
        if (candidate == candidate.root_path()) {
            break;
        }
    }
    return std::filesystem::current_path();
}

std::optional<std::string> TelemetryReader::latestPacketLine(const std::filesystem::path& path) {
    std::ifstream file(path);
    if (!file) {
        return std::nullopt;
    }
    std::optional<std::string> latest;
    std::string line;
    while (std::getline(file, line)) {
        const auto pos = line.find("$SIMCAM,");
        if (pos != std::string::npos) {
            latest = line.substr(pos);
        }
    }
    return latest;
}

bool TelemetryReader::isFresh(const std::filesystem::path& path, std::chrono::seconds maxAge) {
    const auto lastWrite = std::filesystem::last_write_time(path);
    const auto now = std::filesystem::file_time_type::clock::now();
    return (now - lastWrite) <= maxAge;
}
