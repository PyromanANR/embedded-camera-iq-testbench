#include "HttpServer.hpp"

#include <cstring>
#include <algorithm>
#include <cctype>
#include <iostream>
#include <stdexcept>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#endif

namespace {
void closeSocket(int socketFd) {
#ifdef _WIN32
    closesocket(socketFd);
#else
    close(socketFd);
#endif
}
}

HttpServer::HttpServer(std::string host, int port, CameraDevice& device)
    : host_(std::move(host)), port_(port), device_(device) {}

int HttpServer::run() {
#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed\n";
        return 1;
    }
#endif

    int serverFd = static_cast<int>(socket(AF_INET, SOCK_STREAM, 0));
    if (serverFd < 0) {
        std::cerr << "socket() failed\n";
        return 1;
    }

    sockaddr_in address {};
    address.sin_family = AF_INET;
    address.sin_port = htons(static_cast<unsigned short>(port_));
    address.sin_addr.s_addr = inet_addr(host_.c_str());

    int opt = 1;
    setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char*>(&opt), sizeof(opt));

    if (bind(serverFd, reinterpret_cast<sockaddr*>(&address), sizeof(address)) < 0) {
        std::cerr << "bind() failed. Is port " << port_ << " busy?\n";
        closeSocket(serverFd);
        return 1;
    }
    if (listen(serverFd, 8) < 0) {
        std::cerr << "listen() failed\n";
        closeSocket(serverFd);
        return 1;
    }

    std::cout << "SIMCAM C++ gateway listening on http://" << host_ << ":" << port_ << "\n";
    while (running_) {
        sockaddr_in clientAddress {};
#ifdef _WIN32
        int addressLength = sizeof(clientAddress);
#else
        socklen_t addressLength = sizeof(clientAddress);
#endif
        int clientFd = static_cast<int>(accept(serverFd, reinterpret_cast<sockaddr*>(&clientAddress), &addressLength));
        if (clientFd < 0) {
            continue;
        }
        char buffer[4096] = {};
        int received = static_cast<int>(recv(clientFd, buffer, sizeof(buffer) - 1, 0));
        if (received > 0) {
            std::string request(buffer, received);
            while (expectedRequestSize(request) > request.size()) {
                received = static_cast<int>(recv(clientFd, buffer, sizeof(buffer), 0));
                if (received <= 0) {
                    break;
                }
                request.append(buffer, received);
            }
            std::string httpResponse = handleRequest(request);
            send(clientFd, httpResponse.c_str(), static_cast<int>(httpResponse.size()), 0);
        }
        closeSocket(clientFd);
    }

    closeSocket(serverFd);
#ifdef _WIN32
    WSACleanup();
#endif
    return 0;
}

std::string HttpServer::handleRequest(const std::string& request) {
    try {
        if (request.rfind("GET /health", 0) == 0) {
            return response("200 OK", "application/json", device_.healthJson());
        }
        if (request.rfind("GET /telemetry", 0) == 0) {
            return response("200 OK", "application/json", device_.telemetryJson());
        }
        if (request.rfind("GET /snapshot.jpg", 0) == 0) {
            return binaryResponse("200 OK", "image/jpeg", frameProvider_.snapshotJpeg(device_.activeMode()));
        }
        if (request.rfind("POST /control/mode", 0) == 0) {
            return response("200 OK", "application/json", device_.setMode(extractMode(request)));
        }
        if (request.rfind("POST /control/reset", 0) == 0) {
            return response("200 OK", "application/json", device_.reset());
        }
        if (request.rfind("GET /dev/photo-paths", 0) == 0) {
            return response("200 OK", "application/json", frameProvider_.photoPathsJson());
        }
        return response("404 Not Found", "application/json", "{\"error\":\"not found\"}");
    } catch (const std::exception& ex) {
        return response("400 Bad Request", "application/json", std::string("{\"error\":\"") + ex.what() + "\"}");
    }
}

std::string HttpServer::response(const std::string& status, const std::string& contentType, const std::string& body) {
    return "HTTP/1.1 " + status + "\r\n"
        + "Content-Type: " + contentType + "\r\n"
        + "Content-Length: " + std::to_string(body.size()) + "\r\n"
        + "Connection: close\r\n\r\n"
        + body;
}

std::string HttpServer::binaryResponse(
    const std::string& status,
    const std::string& contentType,
    const std::vector<std::uint8_t>& body) {
    std::string header = "HTTP/1.1 " + status + "\r\n"
        + "Content-Type: " + contentType + "\r\n"
        + "Content-Length: " + std::to_string(body.size()) + "\r\n"
        + "Cache-Control: no-store\r\n"
        + "Connection: close\r\n\r\n";
    std::string result;
    result.reserve(header.size() + body.size());
    result.append(header);
    result.append(reinterpret_cast<const char*>(body.data()), body.size());
    return result;
}

std::string HttpServer::extractMode(const std::string& request) {
    const std::string key = "\"mode\"";
    auto keyPos = request.find(key);
    if (keyPos == std::string::npos) {
        throw std::invalid_argument("Missing mode");
    }
    auto colon = request.find(':', keyPos);
    auto firstQuote = request.find('"', colon + 1);
    auto secondQuote = request.find('"', firstQuote + 1);
    if (colon == std::string::npos || firstQuote == std::string::npos || secondQuote == std::string::npos) {
        throw std::invalid_argument("Malformed mode JSON");
    }
    return request.substr(firstQuote + 1, secondQuote - firstQuote - 1);
}

std::size_t HttpServer::expectedRequestSize(const std::string& request) {
    const auto headerEnd = request.find("\r\n\r\n");
    if (headerEnd == std::string::npos) {
        return request.size() + 1;
    }

    std::string headers = request.substr(0, headerEnd);
    std::transform(headers.begin(), headers.end(), headers.begin(),
        [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    const std::string key = "content-length:";
    const auto pos = headers.find(key);
    if (pos == std::string::npos) {
        return headerEnd + 4;
    }
    auto valueStart = pos + key.size();
    while (valueStart < headers.size() && std::isspace(static_cast<unsigned char>(headers[valueStart]))) {
        valueStart++;
    }
    auto valueEnd = valueStart;
    while (valueEnd < headers.size() && std::isdigit(static_cast<unsigned char>(headers[valueEnd]))) {
        valueEnd++;
    }
    const std::size_t contentLength = static_cast<std::size_t>(
        std::stoul(headers.substr(valueStart, valueEnd - valueStart)));
    return headerEnd + 4 + contentLength;
}
