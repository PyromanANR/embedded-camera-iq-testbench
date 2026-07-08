#include <iostream>

#include "CameraDevice.hpp"
#include "HttpServer.hpp"

int main() {
    CameraDevice device;
    HttpServer server("127.0.0.1", 8081, device);
    return server.run();
}
