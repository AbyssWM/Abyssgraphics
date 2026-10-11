#include <abyssgraphics/resource/resources/StaticImageResource.hpp>
#include <abyssutils/memory/Atomic.hpp>
#include <abyssutils/memory/Casts.hpp>

#include <cairo/cairo.h>
#include <pango/pangocairo.h>

using namespace Abyssgraphics;
using namespace Abyssutils::Memory;

CStaticImageResource::CStaticImageResource(const std::span<const uint8_t> data, eImageFormat format) : m_data(data), m_format(format) {
    ;
}

void CStaticImageResource::render() {
    auto image = CImage(m_data, m_format);

    m_asset.cairoSurface = image.cairoSurface();
    m_asset.pixelSize    = m_asset.cairoSurface && m_asset.cairoSurface->cairo() ? m_asset.cairoSurface->size() : Abyssutils::Math::Vector2D{};
}
