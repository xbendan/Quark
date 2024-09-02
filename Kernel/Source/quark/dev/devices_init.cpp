#include <quark/api/logging.h>
#include <quark/os/main.h>

namespace Quark {
    Res<> setupDevices()
    {
        log("Setting up devices...");
        return Ok();
    }
}