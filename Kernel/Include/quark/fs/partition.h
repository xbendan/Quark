#include <mixins/std/c++types.h>

namespace Quark::System::Io::FileSystem {
    class Partition
    {
    public:
        constexpr bool operator==(const Partition& other) const
        {
            return _id == other._id;
        }

    private:
        u8 _id;
    };
} // namespace Quark::System::Io::FileSystem