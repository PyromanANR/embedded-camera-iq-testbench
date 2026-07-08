#include "FrameProvider.hpp"

#include "ImageDegradation.hpp"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iterator>
#include <sstream>
#include <stdexcept>
#include <utility>

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <wincodec.h>
#endif

namespace {
const char* modes[] = {
    "stock", "blurry", "noisy", "overexposed", "underexposed",
    "low_contrast", "color_cast", "jpeg_artifacts", "dead_pixels", "low_light"
};

bool isImageExtension(const std::filesystem::path& path) {
    auto ext = path.extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return ext == ".jpg" || ext == ".jpeg" || ext == ".png" || ext == ".bmp";
}

#ifdef _WIN32
template <typename T>
void releaseCom(T*& ptr) {
    if (ptr) {
        ptr->Release();
        ptr = nullptr;
    }
}

class ComScope {
public:
    ComScope() {
        hr_ = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    }

    ~ComScope() {
        if (SUCCEEDED(hr_)) {
            CoUninitialize();
        }
    }

    void validate() const {
        if (FAILED(hr_) && hr_ != RPC_E_CHANGED_MODE) {
            throw std::runtime_error("COM initialization failed");
        }
    }

private:
    HRESULT hr_;
};

void checkHr(HRESULT hr, const char* message) {
    if (FAILED(hr)) {
        throw std::runtime_error(message);
    }
}

RgbImage decodeImageWindows(const std::filesystem::path& path) {
    ComScope com;
    com.validate();

    IWICImagingFactory* factory = nullptr;
    IWICBitmapDecoder* decoder = nullptr;
    IWICBitmapFrameDecode* frame = nullptr;
    IWICFormatConverter* converter = nullptr;

    try {
        checkHr(CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER,
            IID_PPV_ARGS(&factory)), "WIC factory creation failed");
        checkHr(factory->CreateDecoderFromFilename(path.wstring().c_str(), nullptr, GENERIC_READ,
            WICDecodeMetadataCacheOnLoad, &decoder), "WIC decoder creation failed");
        checkHr(decoder->GetFrame(0, &frame), "WIC frame read failed");
        checkHr(factory->CreateFormatConverter(&converter), "WIC converter creation failed");
        checkHr(converter->Initialize(frame, GUID_WICPixelFormat24bppRGB, WICBitmapDitherTypeNone,
            nullptr, 0.0, WICBitmapPaletteTypeCustom), "WIC RGB conversion failed");

        UINT width = 0;
        UINT height = 0;
        checkHr(converter->GetSize(&width, &height), "WIC image size failed");
        RgbImage image;
        image.width = static_cast<int>(width);
        image.height = static_cast<int>(height);
        image.pixels.resize(static_cast<std::size_t>(width * height * 3));
        checkHr(converter->CopyPixels(nullptr, width * 3, static_cast<UINT>(image.pixels.size()),
            image.pixels.data()), "WIC pixel copy failed");

        releaseCom(converter);
        releaseCom(frame);
        releaseCom(decoder);
        releaseCom(factory);
        return image;
    } catch (...) {
        releaseCom(converter);
        releaseCom(frame);
        releaseCom(decoder);
        releaseCom(factory);
        throw;
    }
}

std::vector<std::uint8_t> encodeJpegWindows(const RgbImage& image, unsigned long quality) {
    ComScope com;
    com.validate();

    const auto tempPath = std::filesystem::temp_directory_path() /
        ("simcam_snapshot_" + std::to_string(GetCurrentProcessId()) + ".jpg");

    IWICImagingFactory* factory = nullptr;
    IWICStream* stream = nullptr;
    IWICBitmapEncoder* encoder = nullptr;
    IWICBitmapFrameEncode* frame = nullptr;
    IPropertyBag2* propertyBag = nullptr;

    try {
        checkHr(CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER,
            IID_PPV_ARGS(&factory)), "WIC factory creation failed");
        checkHr(factory->CreateStream(&stream), "WIC stream creation failed");
        checkHr(stream->InitializeFromFilename(tempPath.wstring().c_str(), GENERIC_WRITE),
            "WIC stream file initialization failed");
        checkHr(factory->CreateEncoder(GUID_ContainerFormatJpeg, nullptr, &encoder),
            "WIC JPEG encoder creation failed");
        checkHr(encoder->Initialize(stream, WICBitmapEncoderNoCache), "WIC encoder initialization failed");
        checkHr(encoder->CreateNewFrame(&frame, &propertyBag), "WIC frame creation failed");
        if (propertyBag) {
            PROPBAG2 option = {};
            option.pstrName = const_cast<LPOLESTR>(L"ImageQuality");
            VARIANT value;
            VariantInit(&value);
            value.vt = VT_R4;
            value.fltVal = std::clamp(static_cast<float>(quality) / 100.0f, 0.05f, 1.0f);
            propertyBag->Write(1, &option, &value);
            VariantClear(&value);
        }
        checkHr(frame->Initialize(propertyBag), "WIC frame initialization failed");
        checkHr(frame->SetSize(static_cast<UINT>(image.width), static_cast<UINT>(image.height)),
            "WIC frame size failed");

        WICPixelFormatGUID pixelFormat = GUID_WICPixelFormat24bppBGR;
        checkHr(frame->SetPixelFormat(&pixelFormat), "WIC pixel format failed");

        std::vector<std::uint8_t> bgr(image.pixels.size());
        for (std::size_t i = 0; i < image.pixels.size(); i += 3) {
            bgr[i + 0] = image.pixels[i + 2];
            bgr[i + 1] = image.pixels[i + 1];
            bgr[i + 2] = image.pixels[i + 0];
        }
        const UINT stride = static_cast<UINT>(image.width * 3);
        checkHr(frame->WritePixels(static_cast<UINT>(image.height), stride,
            static_cast<UINT>(bgr.size()), bgr.data()), "WIC pixel write failed");
        checkHr(frame->Commit(), "WIC frame commit failed");
        checkHr(encoder->Commit(), "WIC encoder commit failed");
    } catch (...) {
        releaseCom(propertyBag);
        releaseCom(frame);
        releaseCom(encoder);
        releaseCom(stream);
        releaseCom(factory);
        std::filesystem::remove(tempPath);
        throw;
    }

    releaseCom(propertyBag);
    releaseCom(frame);
    releaseCom(encoder);
    releaseCom(stream);
    releaseCom(factory);

    std::ifstream file(tempPath, std::ios::binary);
    std::vector<std::uint8_t> result;
    for (std::istreambuf_iterator<char> it(file), end; it != end; ++it) {
        result.push_back(static_cast<std::uint8_t>(*it));
    }
    file.close();
    std::filesystem::remove(tempPath);
    if (result.empty()) {
        throw std::runtime_error("Generated JPEG is empty");
    }
    return result;
}
#endif
}

FrameProvider::FrameProvider(std::filesystem::path projectRoot)
    : projectRoot_(projectRoot.empty() ? locateProjectRoot(std::filesystem::current_path()) : std::move(projectRoot)),
      framesRoot_(projectRoot_ / "data" / "frames") {}

std::vector<std::uint8_t> FrameProvider::snapshotJpeg(const std::string& mode) const {
    const auto modeFiles = imageFiles(mode);
    if (!modeFiles.empty()) {
        return readFileBytes(modeFiles.front());
    }

    const auto stockFiles = imageFiles("stock");
    if (!stockFiles.empty()) {
        if (mode == "stock") {
            return readFileBytes(stockFiles.front());
        }
#ifdef _WIN32
        const RgbImage stock = decodeImageWindows(stockFiles.front());
        const RgbImage degraded = ImageDegradation::apply(stock, mode);
        const unsigned long quality = mode == "jpeg_artifacts" ? 18UL : 88UL;
        return encodeJpegWindows(degraded, quality);
#endif
    }

    return encodeGeneratedJpeg(mode);
}

std::string FrameProvider::photoPathsJson() const {
    std::ostringstream json;
    json << "{";
    for (std::size_t i = 0; i < std::size(modes); ++i) {
        if (i > 0) {
            json << ",";
        }
        json << "\"" << modes[i] << "\":\"data/frames/" << modes[i] << "\"";
    }
    json << "}";
    return json.str();
}

std::filesystem::path FrameProvider::locateProjectRoot(std::filesystem::path start) {
    for (auto candidate = std::move(start); !candidate.empty(); candidate = candidate.parent_path()) {
        if (std::filesystem::exists(candidate / "data" / "frames") &&
            std::filesystem::exists(candidate / "camera_gateway_cpp")) {
            return candidate;
        }
        if (candidate == candidate.root_path()) {
            break;
        }
    }
    return std::filesystem::current_path();
}

std::vector<std::filesystem::path> FrameProvider::imageFiles(const std::string& mode) const {
    std::vector<std::filesystem::path> files;
    const auto folder = framesRoot_ / mode;
    if (!std::filesystem::exists(folder)) {
        return files;
    }
    for (const auto& entry : std::filesystem::directory_iterator(folder)) {
        if (entry.is_regular_file() && isImageExtension(entry.path())) {
            files.push_back(entry.path());
        }
    }
    std::sort(files.begin(), files.end());
    return files;
}

std::vector<std::uint8_t> FrameProvider::readFileBytes(const std::filesystem::path& path) const {
    std::ifstream file(path, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Failed to open image file");
    }
    return {std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()};
}

std::vector<std::uint8_t> FrameProvider::encodeGeneratedJpeg(const std::string& mode) const {
    const RgbImage base = ImageDegradation::createPlaceholder();
    const RgbImage degraded = ImageDegradation::apply(base, mode);
    const unsigned long quality =
#ifdef _WIN32
        mode == "jpeg_artifacts" ? 18UL : 88UL;
    return encodeJpegWindows(degraded, static_cast<ULONG>(quality));
#else
        88UL;
    (void)quality;
    // Minimal valid 1x1 JPEG fallback for non-Windows builds without an image encoder.
    return {
        0xFF,0xD8,0xFF,0xDB,0x00,0x43,0x00,0x08,0x06,0x06,0x07,0x06,0x05,0x08,0x07,0x07,
        0x07,0x09,0x09,0x08,0x0A,0x0C,0x14,0x0D,0x0C,0x0B,0x0B,0x0C,0x19,0x12,0x13,0x0F,
        0x14,0x1D,0x1A,0x1F,0x1E,0x1D,0x1A,0x1C,0x1C,0x20,0x24,0x2E,0x27,0x20,0x22,0x2C,
        0x23,0x1C,0x1C,0x28,0x37,0x29,0x2C,0x30,0x31,0x34,0x34,0x34,0x1F,0x27,0x39,0x3D,
        0x38,0x32,0x3C,0x2E,0x33,0x34,0x32,0xFF,0xC0,0x00,0x11,0x08,0x00,0x01,0x00,0x01,
        0x03,0x01,0x22,0x00,0x02,0x11,0x01,0x03,0x11,0x01,0xFF,0xC4,0x00,0x14,0x00,0x01,
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x08,
        0xFF,0xC4,0x00,0x14,0x10,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xDA,0x00,0x0C,0x03,0x01,0x00,0x02,0x11,0x03,
        0x11,0x00,0x3F,0x00,0xB2,0xC0,0x07,0xFF,0xD9
    };
#endif
}
