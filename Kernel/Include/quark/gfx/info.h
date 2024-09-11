#pragma once

#include <mixins/math/vec.h>

namespace Quark::System::Graphics {
    using Point = Vec<u32, 2>;
    using Size  = Vec<u32, 2>;

    struct IVideoAttribute
    {
        virtual Size const& getResolution()                       = 0;
        virtual bool        setResolution(Size const& resolution) = 0;

        virtual bool const& getIsInterlaced() = 0;

        virtual u32 const& getRefreshRate()                = 0;
        virtual bool       setRefreshRate(u32 refreshRate) = 0;
    };
}