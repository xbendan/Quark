#pragma once

#include <mixins/gfx/color_format.h>
#include <mixins/math/vec.h>
#include <quark/dev/device.h>
#include <quark/gfx/info.h>

namespace Quark::System::Graphics {
    class IVideoDisplayDevice : public Io::Device
    {
    public:
        IVideoDisplayDevice(string name, Size resolution)
            : Device(name, Device::Type::DisplayOutput)
            , m_resolution(resolution)
        {
        }

    private:
        Size m_resolution;
    };
}