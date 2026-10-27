#include "Bmp.hpp"

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <cstddef>
#include <filesystem>
#include <optional>
#include <fstream>
#include <vector>
#include <cstring>

class BmpHeader {
  public:
    unsigned char              format[2];
    uint32_t                   sizeOfFile;
    uint16_t                   reserved1;
    uint16_t                   reserved2;
    uint32_t                   dataOffset;
    uint32_t                   sizeOfBitmapHeader;
    uint32_t                   width;
    uint32_t                   height;
    uint16_t                   numberOfColors;
    uint16_t                   numberOfBitPerPixel;
    uint32_t                   compressionMethod;
    uint32_t                   imageSize;
    uint32_t                   horizontalResolutionPPM;
    uint32_t                   verticalResolutionPPM;
    uint32_t                   numberOfCollors;
    uint32_t                   numberOfImportantCollors;

    std::optional<std::string> load(std::ifstream& file) {
        file.seekg(0, std::ios::end);
        uint32_t streamLength = file.tellg();
        file.seekg(0, std::ios::beg);

        file.read(reinterpret_cast<char*>(&format), sizeof(format));
        if (format[0] != 66 || format[1] != 77)
            return "Unable to parse bitmap header: wrong bmp file type";

        file.read(reinterpret_cast<char*>(&sizeOfFile), sizeof(sizeOfFile));

        if (sizeOfFile != streamLength)
            return "Unable to parse bitmap header: wrong value of file size header";

        file.read(reinterpret_cast<char*>(&reserved1), sizeof(reserved1));
        file.read(reinterpret_cast<char*>(&reserved2), sizeof(reserved2));
        file.read(reinterpret_cast<char*>(&dataOffset), sizeof(dataOffset));
        file.read(reinterpret_cast<char*>(&sizeOfBitmapHeader), sizeof(sizeOfBitmapHeader));
        file.read(reinterpret_cast<char*>(&width), sizeof(width));
        file.read(reinterpret_cast<char*>(&height), sizeof(height));
        file.read(reinterpret_cast<char*>(&numberOfColors), sizeof(numberOfColors));
        file.read(reinterpret_cast<char*>(&numberOfBitPerPixel), sizeof(numberOfBitPerPixel));
        file.read(reinterpret_cast<char*>(&compressionMethod), sizeof(compressionMethod));
        file.read(reinterpret_cast<char*>(&imageSize), sizeof(imageSize));
        file.read(reinterpret_cast<char*>(&horizontalResolutionPPM), sizeof(horizontalResolutionPPM));
        file.read(reinterpret_cast<char*>(&verticalResolutionPPM), sizeof(verticalResolutionPPM));
        file.read(reinterpret_cast<char*>(&numberOfCollors), sizeof(numberOfCollors));
        file.read(reinterpret_cast<char*>(&numberOfImportantCollors), sizeof(numberOfImportantCollors));

        if (!file || dataOffset >= streamLength)
            return "Unable to parse bitmap header: truncated file";

        // BI_RGB, or BI_BITFIELDS with the usual masks for 32 bit
        if (compressionMethod != 0 && !(compressionMethod == 3 && numberOfBitPerPixel == 32))
            return "Unable to parse bitmap header: compressed bitmaps are not supported";

        file.seekg(dataOffset);

        return std::nullopt;
    };
};

std::expected<cairo_surface_t*, std::string> BMP::createSurfaceFromBMP(const std::string& path) {
    if (!std::filesystem::exists(path))
        return std::unexpected("loading bmp: file doesn't exist");

    std::ifstream bitmapImageStream(path, std::ios::binary);
    BmpHeader     bitmapHeader;
    if (const auto RET = bitmapHeader.load(bitmapImageStream); RET.has_value())
        return std::unexpected("loading bmp: " + *RET);

    const auto BPP = bitmapHeader.numberOfBitPerPixel;
    if (BPP != 24 && BPP != 32)
        return std::unexpected("loading bmp: unsupported bmp stream");

    // a negative height stores the rows top-down
    const int32_t  WIDTH   = static_cast<int32_t>(bitmapHeader.width);
    const int32_t  HEIGHT  = static_cast<int32_t>(bitmapHeader.height);
    const bool     TOPDOWN = HEIGHT < 0;
    const uint32_t ROWS    = TOPDOWN ? -static_cast<int64_t>(HEIGHT) : HEIGHT;
    if (WIDTH <= 0 || WIDTH > 32767 || ROWS == 0 || ROWS > 32767)
        return std::unexpected("loading bmp: invalid image size");

    auto* surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, WIDTH, ROWS);
    if (cairo_surface_status(surface) != CAIRO_STATUS_SUCCESS) {
        cairo_surface_destroy(surface);
        return std::unexpected("loading bmp: failed to create a surface");
    }

    cairo_surface_flush(surface);
    unsigned char* data   = cairo_image_surface_get_data(surface);
    const int      STRIDE = cairo_image_surface_get_stride(surface);

    // rows in the file are padded to 4 bytes
    const size_t               ROWBYTES = ((static_cast<size_t>(WIDTH) * BPP + 31) / 32) * 4;
    std::vector<unsigned char> row(ROWBYTES);
    bool                       anyAlpha = false;

    for (uint32_t r = 0; r < ROWS; ++r) {
        if (!bitmapImageStream.read(reinterpret_cast<char*>(row.data()), ROWBYTES)) {
            cairo_surface_destroy(surface);
            return std::unexpected("loading bmp: file is shorter than the image");
        }

        auto* dst = reinterpret_cast<uint32_t*>(data + static_cast<size_t>(TOPDOWN ? r : ROWS - 1 - r) * STRIDE);
        for (int32_t x = 0; x < WIDTH; ++x) {
            const unsigned char* px = &row[static_cast<size_t>(x) * (BPP / 8)];
            const uint32_t       A  = BPP == 32 ? px[3] : 0xFF;
            anyAlpha |= A != 0;
            // cairo wants premultiplied alpha
            dst[x] = (A << 24) | ((px[2] * A / 255) << 16) | ((px[1] * A / 255) << 8) | (px[0] * A / 255);
        }
    }

    // many 32 bit bitmaps leave the alpha byte at 0: those are opaque
    if (BPP == 32 && !anyAlpha) {
        bitmapImageStream.clear();
        bitmapImageStream.seekg(bitmapHeader.dataOffset);
        for (uint32_t r = 0; r < ROWS; ++r) {
            bitmapImageStream.read(reinterpret_cast<char*>(row.data()), ROWBYTES);
            auto* dst = reinterpret_cast<uint32_t*>(data + static_cast<size_t>(TOPDOWN ? r : ROWS - 1 - r) * STRIDE);
            for (int32_t x = 0; x < WIDTH; ++x) {
                const unsigned char* px = &row[static_cast<size_t>(x) * 4];
                dst[x]                  = 0xFF000000u | (px[2] << 16) | (px[1] << 8) | px[0];
            }
        }
    }

    cairo_surface_mark_dirty(surface);
    return surface;
}
