#include <quark/dev/device.h>

namespace VmWare {
    using namespace Quark::System;

    class ControllerDevice : public Io::Device
    {
    public:
        Res<> onLoad() override;
        Res<> onStartup() override;
    };
}