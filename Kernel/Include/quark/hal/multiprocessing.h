#pragma once

#include <mixins/meta/result.h>
#include <mixins/utils/collections.h>
#include <quark/dev/device.h>
#include <quark/sched/thread.h>

namespace Quark::System::Hal {
    enum class Signal
    {
        INIT    = 0,
        STARTUP = 1,
        HALT    = 2,
        RESET   = 3,
        SCHED   = 4,
        IPI     = 5,
        EOI     = 6,
    };

    struct ICPULocalDevice : public Io::Device
    {
        u32 _id;

        ICPULocalDevice(u32 id)
            : Device("Logical Processor Device", Type::Processor)
            , _id(id)
        {
        }

        virtual void SendSignal(Signal signal, u32 data = 0);

        IQueue<Task::Thread*>* _threadQueue;
    };

    Res<ICollection<ICPULocalDevice*>*> SetupMultiprocessing();

    ICPULocalDevice* GetCPULocal(u32 id);
    ICPULocalDevice* getCPULocal();
    void             SetCPULocal(u32 id, ICPULocalDevice* local);
    void             SetCPULocal(ICPULocalDevice* local);
}