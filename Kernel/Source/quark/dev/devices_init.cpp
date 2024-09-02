#include <quark/api/logging.h>
#include <quark/os/main.h>

namespace Quark::System {
    Res<> setupDevices()
    {
        log(u8"Setting up devices...");
        return Ok();
    }
}