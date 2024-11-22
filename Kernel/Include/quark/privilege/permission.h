#include <quark/privilege/scene.h>

namespace Quark::System::UserManagement {
    enum class AppPerm
    {
        CAMARA,
        AUDIO_RECORDING,
        SPEECH_RECOGNITION,
        LOCATION_ACCESS,
    };

    enum class AppPermState
    {
        PROMPT,     // Let user decide
        ALLOWED,    // Allow all the time
        ALLOW_ONCE, // Allow once
        FORGE,      // Forge the required data
        DENIED,     // Deny and don't ask again
    };
}