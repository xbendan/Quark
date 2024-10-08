#include <quark/privilege/scene.h>

namespace Quark::System::Privilege {
    enum class Permission
    {
        Camera,
        Microphone,
        Location,
        FileAccess,
        LibraryAccess,
    };

    struct PermissionInstance
    {
        Permission _permission;
        Scene      _scene;
    };
}