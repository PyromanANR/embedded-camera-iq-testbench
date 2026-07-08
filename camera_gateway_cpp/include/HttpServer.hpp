#pragma once

#include <atomic>
#include <cstdint>
#include <string>
#include <vector>

#include "CameraDevice.hpp"
#include "FrameProvider.hpp"

class HttpServer {
public:
    HttpServer(std::string host, int port, CameraDevice& device);
    int run();

private:
    std::string handleRequest(const std::string& request);
    static std::string response(const std::string& status, const std::string& contentType, const std::string& body);
    static std::string binaryResponse(
        const std::string& status,
        const std::string& contentType,
        const std::vector<std::uint8_t>& body);
    static std::string extractMode(const std::string& request);
    static std::size_t expectedRequestSize(const std::string& request);

    std::string host_;
    int port_;
    CameraDevice& device_;
    FrameProvider frameProvider_;
    std::atomic<bool> running_{true};
};
