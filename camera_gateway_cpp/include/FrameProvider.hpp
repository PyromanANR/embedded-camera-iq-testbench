#pragma once

#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

class FrameProvider {
public:
    explicit FrameProvider(std::filesystem::path projectRoot = {});

    std::vector<std::uint8_t> snapshotJpeg(const std::string& mode) const;
    std::string photoPathsJson() const;

private:
    std::filesystem::path projectRoot_;
    std::filesystem::path framesRoot_;

    static std::filesystem::path locateProjectRoot(std::filesystem::path start);
    std::vector<std::filesystem::path> imageFiles(const std::string& mode) const;
    std::vector<std::uint8_t> readFileBytes(const std::filesystem::path& path) const;
    std::vector<std::uint8_t> encodeGeneratedJpeg(const std::string& mode) const;
};
