#pragma once

#include <string>
#include <span>
#include <cairo/cairo.h>
#include "../cairo/CairoSurface.hpp"
#include <abyssutils/memory/SharedPtr.hpp>

namespace Abyssgraphics {
    enum eImageFormat : uint8_t {
        IMAGE_FORMAT_PNG,
        IMAGE_FORMAT_AVIF,
        IMAGE_FORMAT_JPEG,
        IMAGE_FORMAT_JXL,
        IMAGE_FORMAT_BMP,
        IMAGE_FORMAT_SVG,
        IMAGE_FORMAT_WEBP,

        IMAGE_FORMAT_ERROR,
        IMAGE_FORMAT_AUTO, // take an educated guess
    };

    class CImage {
      public:
        CImage(const std::string& path, const Abyssutils::Math::Vector2D& size = {} /* Ignored if not svg */);
        CImage(std::span<const uint8_t>, eImageFormat format = IMAGE_FORMAT_AUTO, const Abyssutils::Math::Vector2D& size = {} /* Ignored if not svg */);
        ~CImage();

        CImage(const CImage&)            = delete;
        CImage& operator=(const CImage&) = delete;

        //
        bool                                             success();
        bool                                             hasAlpha();
        std::string                                      getError();
        std::string                                      getMime();

        Abyssutils::Memory::CSharedPointer<CCairoSurface> cairoSurface();

        static bool                                      isImageFile(const std::string& path);

      private:
        std::string                                      lastError, filepath, mime;
        Abyssutils::Math::Vector2D                        m_svgSize;
        Abyssutils::Memory::CSharedPointer<CCairoSurface> pCairoSurface;
        bool                                             imageHasAlpha = true, loadSuccess = false;
    };
};
