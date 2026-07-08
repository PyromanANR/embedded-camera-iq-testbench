#pragma once

#include <chrono>
#include <filesystem>
#include <optional>
#include <string>

#include "TelemetryModel.hpp"

struct TelemetryReadResult {
    bool fresh = false;
    bool valid = false;
    std::string warning;
    std::optional<Telemetry> telemetry;
};

class TelemetryReader {
public:
    explicit TelemetryReader(std::filesystem::path logPath = {});

    TelemetryReadResult readLatest() const;
    std::filesystem::path logPath() const;

private:
    std::filesystem::path logPath_;

    static std::filesystem::path locateProjectRoot(std::filesystem::path start);
    static std::optional<std::string> latestPacketLine(const std::filesystem::path& path);
    static bool isFresh(const std::filesystem::path& path, std::chrono::seconds maxAge);
};

