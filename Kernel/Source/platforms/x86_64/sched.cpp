#include <platforms/x86_64/address_space.h>
#include <platforms/x86_64/cpu.h>
#include <platforms/x86_64/sched.h>
#include <quark/sched/process.h>

namespace Quark::System::Platform::X64 {
    ThreadEx::ThreadEx(Task::Process* process)
        : Task::Thread(process, process->GetNextThreadId())
        , _stackKernelBase(new u8[65536])
        , _stackKernel((void*)((u64)_stackKernelBase + 65536))
        , _fxState(new FxState())
    {
        _registers.rflags = 0x202;
        _registers.cs     = 0x8;
        _registers.ss     = 0x10;

        _fxState->_fcontrol  = 0x33f;
        _fxState->_mxcsr     = 0x1f80;
        _fxState->_mxcsrMask = 0xffbf;
    }
}

namespace Quark::System::Task {
    using namespace Quark::System::Platform::X64;

    void Scheduler::ScheduleToThread(ProcessContext* ctx, Thread* thread)
    {
        ThreadEx* t = static_cast<ThreadEx*>(thread);

        asm volatile("fxrstor64 (%0)" ::"r"((u64)t->_fxState) : "memory");
        wrmsr(MSR_FS_BASE, (u64)t->_fsBase);

        ctx->_currentThread         = t;
        GetCPULocal()->_tss._rsp[0] = reinterpret_cast<u64>(t->_stackKernel);

        asm volatile(
            R"(mov %0, %%rsp;
            mov %1, %%rax;
            pop %%r15;
            pop %%r14;
            pop %%r13;
            pop %%r12;
            pop %%r11;
            pop %%r10;
            pop %%r9;
            pop %%r8;
            pop %%rbp;
            pop %%rdi;
            pop %%rsi;
            pop %%rdx;
            pop %%rcx;
            pop %%rbx;
            
            mov %%rax, %%cr3
            pop %%rax
            addq $8, %%rsp
            iretq)" ::"r"(&t->_registers),
            "r"(static_cast<X64AddressSpace<Privilege::Level::System>*>(
                    t->GetProcess()->_addressSpace)
                    ->_pml4Phys));
    }

    void Scheduler::ScheduleToNext(ProcessContext* ctx) {}

    void Scheduler::ScheduleAllContexts() {}

    Thread* ProcessFactory::CreateThread(Process* process)
    {
        return new ThreadEx(process);
    }

    Thread* ProcessFactory::CreateThreadEx(Process*      process,
                                           u8            priority,
                                           const string& name,
                                           const string& description,
                                           const string& command,
                                           const string& arguments)
    {
        // TODO: Implement this
        return nullptr;
    }
}