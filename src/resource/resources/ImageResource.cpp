#include <abyssgraphics/image/Image.hpp>
#include <abyssgraphics/resource/resources/ImageResource.hpp>
#include <abyssutils/memory/Atomic.hpp>
#include <abyssutils/memory/Casts.hpp>

#include <cairo/cairo.h>
#include <pango/pangocairo.h>

using namespace Abyssgraphics;
using namespace Abyssutils::Memory;

CImageResource::CImageResource(const std::string& path) : m_path(path) {
    ;
}

CImageResource::CImageResource(const std::string& svg, const Abyssutils::Math::Vector2D& size) : m_path(svg), m_svgSize(size) {
    ;
}

CImageResource::CImageResource(const std::span<const uint8_t>& data, const Abyssutils::Math::Vector2D& size) : m_svgSize(size), m_data(data) {
    ;
}

void CImageResource::render() {
    auto image = !m_data.empty() ? CImage(m_data, IMAGE_FORMAT_AUTO, m_svgSize) : CImage(m_path, m_svgSize);

    m_asset.cairoSurface = image.cairoSurface();
    m_asset.pixelSize    = m_asset.cairoSurface && m_asset.cairoSurface->cairo() ? m_asset.cairoSurface->size() : Abyssutils::Math::Vector2D{};
}
