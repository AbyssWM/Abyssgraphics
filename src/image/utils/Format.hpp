#pragma once

#include <abyssgraphics/image/Image.hpp>

namespace Abyssgraphics {
    eImageFormat formatFromStream(const std::span<const uint8_t>& data);
    eImageFormat formatFromFile(const std::string& path);
    const char*  mimeOf(eImageFormat);
};
