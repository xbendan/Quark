#include <mixins/math/vec.h>
#include <quark/dev/device.h>
#include <quark/gfx/color.h>

namespace Quark::System::Graphics {
    using Point = Vec<u32, 2>;
    using Size  = Vec<u32, 2>;

    class IVideoOutputDevice : public Io::Device
    {
    public:
        IVideoOutputDevice(string name, Size resolution)
            : Device(name, Device::Type::DisplayOutput)
            , m_resolution(resolution)
        {
        }

        virtual void draw(Point const& point, Rgba) = 0;

        virtual Size const& getResolution() const { return m_resolution; }

    private:
        Size m_resolution;
    };
}