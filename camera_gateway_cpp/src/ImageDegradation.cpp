#include "ImageDegradation.hpp"

#include <algorithm>
#include <cmath>
#include <random>

namespace {
std::uint8_t clampByte(int value) {
    return static_cast<std::uint8_t>(std::clamp(value, 0, 255));
}

int indexOf(const RgbImage& image, int x, int y, int channel) {
    return ((y * image.width + x) * 3) + channel;
}
}

RgbImage ImageDegradation::createPlaceholder(int width, int height) {
    RgbImage image;
    image.width = width;
    image.height = height;
    image.pixels.resize(static_cast<std::size_t>(width * height * 3));

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            const double fx = static_cast<double>(x) / static_cast<double>(width);
            const double fy = static_cast<double>(y) / static_cast<double>(height);
            std::uint8_t r = clampByte(static_cast<int>(42 + fx * 130));
            std::uint8_t g = clampByte(static_cast<int>(58 + fy * 120));
            std::uint8_t b = clampByte(static_cast<int>(74 + (1.0 - fx) * 95));

            const bool focusBars = x > width * 0.63 && x < width * 0.88 && y > height * 0.20 && y < height * 0.75;
            if (focusBars && ((x / 12) % 2 == 0)) {
                r = 235;
                g = 238;
                b = 230;
            }

            const int cx = width / 2;
            const int cy = height / 2;
            const int dx = x - cx;
            const int dy = y - cy;
            const int radius = static_cast<int>(std::sqrt(dx * dx + dy * dy));
            if (radius < height / 5) {
                r = 70;
                g = 178;
                b = 212;
            }
            if (radius < height / 11) {
                r = 245;
                g = 247;
                b = 240;
            }

            image.pixels[indexOf(image, x, y, 0)] = r;
            image.pixels[indexOf(image, x, y, 1)] = g;
            image.pixels[indexOf(image, x, y, 2)] = b;
        }
    }
    return image;
}

RgbImage ImageDegradation::apply(const RgbImage& source, const std::string& mode) {
    if (mode == "stock") {
        return source;
    }
    if (mode == "blurry") {
        return blur(source);
    }
    if (mode == "noisy") {
        return addNoise(source, 28.0);
    }
    if (mode == "overexposed") {
        return adjustBrightness(source, 1.45, 65);
    }
    if (mode == "underexposed") {
        return adjustBrightness(source, 0.34, 0);
    }
    if (mode == "low_contrast") {
        return adjustContrast(source, 0.35);
    }
    if (mode == "color_cast") {
        return colorCast(source);
    }
    if (mode == "dead_pixels") {
        return deadPixels(source);
    }
    if (mode == "low_light") {
        return addNoise(adjustBrightness(source, 0.22, 0), 18.0);
    }
    if (mode == "jpeg_artifacts") {
        return adjustContrast(addNoise(source, 5.0), 0.82);
    }
    return source;
}

RgbImage ImageDegradation::blur(const RgbImage& source) {
    RgbImage output = source;
    const int radius = 5;
    for (int y = 0; y < source.height; ++y) {
        for (int x = 0; x < source.width; ++x) {
            int sum[3] = {0, 0, 0};
            int count = 0;
            for (int yy = std::max(0, y - radius); yy <= std::min(source.height - 1, y + radius); ++yy) {
                for (int xx = std::max(0, x - radius); xx <= std::min(source.width - 1, x + radius); ++xx) {
                    for (int c = 0; c < 3; ++c) {
                        sum[c] += source.pixels[indexOf(source, xx, yy, c)];
                    }
                    count++;
                }
            }
            for (int c = 0; c < 3; ++c) {
                output.pixels[indexOf(output, x, y, c)] = clampByte(sum[c] / count);
            }
        }
    }
    return output;
}

RgbImage ImageDegradation::addNoise(const RgbImage& source, double sigma) {
    RgbImage output = source;
    std::mt19937 rng(42);
    std::normal_distribution<double> distribution(0.0, sigma);
    for (auto& value : output.pixels) {
        value = clampByte(static_cast<int>(value) + static_cast<int>(distribution(rng)));
    }
    return output;
}

RgbImage ImageDegradation::adjustBrightness(const RgbImage& source, double scale, int offset) {
    RgbImage output = source;
    for (auto& value : output.pixels) {
        value = clampByte(static_cast<int>(value * scale) + offset);
    }
    return output;
}

RgbImage ImageDegradation::adjustContrast(const RgbImage& source, double scale) {
    RgbImage output = source;
    for (auto& value : output.pixels) {
        value = clampByte(static_cast<int>((static_cast<int>(value) - 128) * scale + 128));
    }
    return output;
}

RgbImage ImageDegradation::colorCast(const RgbImage& source) {
    RgbImage output = source;
    for (std::size_t i = 0; i < output.pixels.size(); i += 3) {
        output.pixels[i + 0] = clampByte(static_cast<int>(output.pixels[i + 0]) - 20);
        output.pixels[i + 1] = clampByte(static_cast<int>(output.pixels[i + 1]) - 8);
        output.pixels[i + 2] = clampByte(static_cast<int>(output.pixels[i + 2]) + 55);
    }
    return output;
}

RgbImage ImageDegradation::deadPixels(const RgbImage& source) {
    RgbImage output = source;
    std::mt19937 rng(43);
    std::uniform_int_distribution<int> xDistribution(0, source.width - 1);
    std::uniform_int_distribution<int> yDistribution(0, source.height - 1);
    std::uniform_int_distribution<int> valueDistribution(0, 1);
    const int count = std::max(200, (source.width * source.height) / 160);
    for (int i = 0; i < count; ++i) {
        const int x = xDistribution(rng);
        const int y = yDistribution(rng);
        const std::uint8_t value = valueDistribution(rng) ? 255 : 0;
        for (int c = 0; c < 3; ++c) {
            output.pixels[indexOf(output, x, y, c)] = value;
        }
    }
    return output;
}

