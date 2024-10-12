#include <mixins/concurrent/atomic.h>
#include <mixins/utils/bitmap.h>

namespace Quark::System::Task {
    class PidNamespace
    {
    public:
        PidNamespace();

        u32 GetRemainingCount() const;
        u32 NextPid();

    private:
        Atomic<u32> m_nextId;
        Bitmap      m_pidmap;
    };
}