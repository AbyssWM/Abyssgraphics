#pragma once

#include <cairo/cairo.h>
#include <string>
#include <expected>
#include <span>
#include <png.h>
#include <abyssutils/math/Vector2D.hpp>

namespace SVG {
    std::expected<cairo_surface_t*, std::string> createSurfaceFromSVG(const std::string&, const Abyssutils::Math::Vector2D& size);
    std::expected<cairo_surface_t*, std::string> createSurfaceFromData(const std::span<const uint8_t>&, const Abyssutils::Math::Vector2D& size);
};
