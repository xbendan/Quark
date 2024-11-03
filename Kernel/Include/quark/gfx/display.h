#pragma once

#include <mixins/gfx/color_format.h>
#include <mixins/math/vec.h>
#include <quark/dev/device.h>
#include <quark/gfx/info.h>

namespace Quark::System::Graphics {
    class IVideoOutputDevice : public Io::Device
    {
    public:
        IVideoOutputDevice(Qk::StringView name, Size resolution)
            : Device(name, Device::Type::DisplayOutput)
            , m_resolution(resolution)
        {
        }

    private:
        Size m_resolution;
    };
}