#pragma once

#include <mixins/meta/result.h>
#include <mixins/utils/collections.h>

namespace Quark::System::Hal {
    struct ICPULocal
    {
        u32 _id;
    };

    Res<> setupCPULocal(u32 id, ICPULocal* local);
    Res<IReadOnlyCollection<ICPULocal*>*> setupMultiprocessing();

    ICPULocal* getCPULocal(u32 id);
    ICPULocal* getCPULocal();
    void       setCPULocal(u32 id, ICPULocal* local);
    void       setCPULocal(ICPULocal* local);
}