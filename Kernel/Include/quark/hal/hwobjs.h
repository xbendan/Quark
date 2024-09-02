#pragma once

#include <quark/dev/device.h>
#include <quark/sched/thread.h>

namespace Quark::Hal {
    using namespace Quark::Task;

    enum class ProcessorSignal
    {
        INIT    = 0,
        STARTUP = 1,
        HALT    = 2,
        RESET   = 3,
        SCHED   = 4,
        IPI     = 5,
        EOI     = 6,
    };

    class ProcessorDevice : public Io::Device
    {
    public:
        ProcessorDevice(u32 processorId, Thread* idleThread);
        ~ProcessorDevice() = delete;

        void sendSignal(ProcessorSignal signal, u32 data = 0);

        u32     getProcessorId() const { return _processorId; }
        Thread* getCurrentThread() { return _currentThread; }
        Thread* getIdleThread() { return _idleThread; }

    private:
        u32     _processorId;
        Thread* _currentThread;
        Thread* _idleThread;
    };
}; // namespace Quark::Hal