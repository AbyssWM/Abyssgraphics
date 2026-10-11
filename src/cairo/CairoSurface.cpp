#include <abyssgraphics/cairo/CairoSurface.hpp>

using namespace Abyssgraphics;

Abyssgraphics::CCairoSurface::CCairoSurface(cairo_surface_t* surf) : pSurface(surf) {
    ;
}

Abyssgraphics::CCairoSurface::~CCairoSurface() {
    if (pSurface)
        cairo_surface_destroy(pSurface);
}

cairo_surface_t* Abyssgraphics::CCairoSurface::cairo() {
    return pSurface;
}

Abyssutils::Math::Vector2D Abyssgraphics::CCairoSurface::size() {
    return {cairo_image_surface_get_width(pSurface), cairo_image_surface_get_height(pSurface)};
}

int Abyssgraphics::CCairoSurface::status() {
    return cairo_surface_status(pSurface);
}

uint8_t* Abyssgraphics::CCairoSurface::data() {
    return (uint8_t*)cairo_image_surface_get_data(pSurface);
}

int Abyssgraphics::CCairoSurface::stride() {
    return cairo_image_surface_get_stride(pSurface);
}
