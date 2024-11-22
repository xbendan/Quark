#pragma once

#include <platforms/x86_64/fpu.h>
#include <platforms/x86_64/hwregs.h>
#include <platforms/x86_64/tables.h>

#include <quark/hal/multiprocessing.h>
#include <quark/sched/sched.h>

#include <mixins/concurrent/atomic.h>

namespace Quark::System::Platform::X64 {
    using Quark::System::Hal::ICPULocalDevice;
    using Quark::System::Task::Thread;

    struct InterruptRetainer
    {
        void acquire()
        {
            asm volatile("cli");
            _count.Inc();
        }

        void release()
        {
            if (_count.Load() || (_count.FetchDec()) == 0) {
                asm volatile("sti");
            }
        }

        Atomic<int> _count{ 0 };
    };

    struct ThreadEx : public Task::Thread
    {
    public:
        ThreadEx(Task::Process*);

        void* _stackUserBase;
        void* _stackUser;
        void* _stackKernelBase;
        void* _stackKernel;

        u64      _fsBase;
        FxState* _fxState;

        Registers _registers;
        Registers _syscall;
    };

    struct CPULocalDevice : ICPULocalDevice
    {
        CPULocalDevice*        _this;
        GlobDescTbl            _gdt;
        GlobDescTbl::Pack      _gdtPtr;
        InterruptDescTbl::Pack _idtPtr;
        TaskStateSegment       _tss __attribute__((aligned(0x10)));
        Task::ProcessContext   _schedule;
        InterruptRetainer      _interruptRetainer;

        CPULocalDevice() = delete;
        CPULocalDevice(u32 id, Thread* idleThread = nullptr)
            : ICPULocalDevice{ id }
            , _this(this)
            , _gdt(&_tss)
            , _gdtPtr({
                  ._size   = sizeof(_gdt) - 1,
                  ._offset = (u64)&_gdt,
              })
            , _idtPtr()
            , _tss()
            , _schedule({ id, nullptr, idleThread })
        {
        }
        ~CPULocalDevice() = default;
    } __attribute__((aligned(0x10)));
} // namespace Quark::System::Platform::X64