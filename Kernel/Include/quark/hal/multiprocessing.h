#pragma once

#include <mixins/meta/result.h>
#include <mixins/utils/collections.h>
#include <mixins/utils/queue.h>
#include <quark/dev/device.h>
#include <quark/sched/thread.h>

namespace Quark::System::Hal {
    using Qk::List;
    using Qk::Queue;

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

        Queue<Task::Thread*>* _threadQueue;
    };

    Res<List<ICPULocalDevice*>*> SetupMultiprocessing();

    ICPULocalDevice* GetCPULocal(u32 id);
    ICPULocalDevice* GetCPULocal();
    void             SetCPULocal(u32 id, ICPULocalDevice* local);
    void             SetCPULocal(ICPULocalDevice* local);

    template <typename T>
    struct CPULocal final
    {
        constexpr CPULocal()
            : _data(nullptr)
        {
        }
        constexpr CPULocal(int amount)
        {
            if (_data == nullptr) {
                _data = new T[amount];
            }
        }
        ~CPULocal()
        {
            if (_data != nullptr) {
                delete[] _data;
            }
        }

        inline T* Get() const
        {
            auto* cpu = GetCPULocal();
            if (_data == nullptr || cpu == nullptr) {
                return nullptr;
            }
            return &_data[cpu->_id];
        }

        const T* Get(int index) const;

        constexpr T* operator()() const { return Get(); }

    private:
        T*    _data;
        usize _align;
    };
}