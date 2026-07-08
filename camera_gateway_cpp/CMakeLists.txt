cmake_minimum_required(VERSION 3.16)
project(simcam_gateway_cpp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(simcam_gateway
    src/main.cpp
    src/CameraDevice.cpp
    src/FrameProvider.cpp
    src/TelemetryModel.cpp
    src/TelemetryReader.cpp
    src/TelemetryParser.cpp
    src/PowerModel.cpp
    src/HttpServer.cpp
    src/ImageDegradation.cpp
)

target_include_directories(simcam_gateway PRIVATE include)

if(WIN32)
    target_link_libraries(simcam_gateway PRIVATE ws2_32 ole32 oleaut32 windowscodecs)
endif()
