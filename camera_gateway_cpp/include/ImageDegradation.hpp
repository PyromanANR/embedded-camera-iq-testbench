#pragma once

#include <cstdint>
#include <string>
#include <vector>

struct RgbImage {
    int width = 0;
    int height = 0;
    std::vector<std::uint8_t> pixels;
};

class ImageDegradation {
public:
    static RgbImage createPlaceholder(int width = 960, int height = 540);
    static RgbImage apply(const RgbImage& source, const std::string& mode);

private:
    static RgbImage blur(const RgbImage& source);
    static RgbImage addNoise(const RgbImage& source, double sigma);
    static RgbImage adjustBrightness(const RgbImage& source, double scale, int offset);
    static RgbImage adjustContrast(const RgbImage& source, double scale);
    static RgbImage colorCast(const RgbImage& source);
    static RgbImage deadPixels(const RgbImage& source);
};

