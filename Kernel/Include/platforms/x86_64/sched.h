#pragma once

#include <platforms/x86_64/hwregs.h>
#include <platforms/x86_64/tables.h>

#include <quark/hal/multiprocessing.h>
#include <quark/hal/task.h>
#include <quark/sched/sched.h>

#include <mixins/concurrent/atomic.h>

namespace Quark::System::Platform::X64 {
    using Quark::System::Hal::ICPULocal;
    using Quark::System::Task::Thread;

    struct InterruptRetainer
    {
        void acquire()
        {
            asm volatile("cli");
            _count.inc();
        }

        void release()
        {
            if (_count.load() || (_count.fetchDec()) == 0) {
                asm volatile("sti");
            }
        }

        Atomic<int> _count{ 0 };
    };

    struct ThreadEx : public Task::Thread
    {
    public:
        void* _stackUser;
        void* _stackUserBase;
        void* _stackKernel;
        void* _stackKernelBase;

        u64   _fsBase;
        void* _fxState;

        Registers _registers;
        Registers _syscall;
    };

    struct CPULocal : ICPULocal
    {
        GlobDescTbl            _gdt;
        GlobDescTbl::Pack      _gdtPtr;
        InterruptDescTbl::Pack _idtPtr;
        TaskStateSegment       _tss __attribute__((aligned(0x10)));
        Task::ProcessContext   _schedule;
        InterruptRetainer      _interruptRetainer;

        CPULocal() = delete;
        CPULocal(u32 id, Thread* idleThread = nullptr)
            : ICPULocal{ id }
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
        ~CPULocal() = default;
    };
} // namespace Quark::System::Platform::X64