#include "TelemetryParser.hpp"

#include <stdexcept>

std::optional<Telemetry> TelemetryParser::parseSimcamPacket(const std::string& packet) {
    if (packet.rfind("$SIMCAM,", 0) != 0) {
        return std::nullopt;
    }

    try {
        Telemetry telemetry;
        telemetry.frameId = std::stoi(fieldValue(packet, "FRAME"));
        telemetry.temperatureC = std::stod(fieldValue(packet, "TEMP"));
        telemetry.batteryPercent = std::stoi(fieldValue(packet, "BAT"));
        telemetry.lux = std::stoi(fieldValue(packet, "LUX"));
        telemetry.exposureMs = std::stod(fieldValue(packet, "EXP"));
        telemetry.sensorGain = std::stod(fieldValue(packet, "GAIN"));
        telemetry.ledMode = fieldValue(packet, "LED");
        telemetry.estimatedPowerMw = std::stoi(fieldValue(packet, "PWR"));
        telemetry.errorCode = std::stoi(fieldValue(packet, "ERR"));
        telemetry.connectionStatus = "firmware_online";
        return telemetry;
    } catch (const std::exception&) {
        return std::nullopt;
    }
}

std::string TelemetryParser::fieldValue(const std::string& packet, const std::string& key) {
    const std::string token = key + "=";
    const auto start = packet.find(token);
    if (start == std::string::npos) {
        throw std::invalid_argument("missing field");
    }
    const auto valueStart = start + token.size();
    const auto valueEnd = packet.find(',', valueStart);
    return packet.substr(valueStart, valueEnd == std::string::npos ? std::string::npos : valueEnd - valueStart);
}

