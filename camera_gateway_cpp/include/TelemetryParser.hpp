#pragma once

#include <optional>
#include <string>

#include "TelemetryModel.hpp"

class TelemetryParser {
public:
    static std::optional<Telemetry> parseSimcamPacket(const std::string& packet);

private:
    static std::string fieldValue(const std::string& packet, const std::string& key);
};

