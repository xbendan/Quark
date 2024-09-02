#include <platforms/x86_64/cpu.h>
#include <platforms/x86_64/paging.h>
#include <platforms/x86_64/sched.h>
#include <quark/api/task.h>
#include <quark/sched/process.h>

namespace Quark::System::Task {
    using namespace Quark::System::Platform::X64;

    void Scheduler::schedule(Scheduler::Context* context, Thread* thread)
    {
        ThreadEx* t = static_cast<ThreadEx*>(thread);

        asm volatile("fxrstor64 (%0)" ::"r"((u64)t->_fxState) : "memory");
        wrmsr(MSR_FS_BASE, (u64)t->_fsBase);

        context->_currentThread     = t;
        getCPULocal()->_tss._rsp[0] = reinterpret_cast<u64>(t->_stackKernel);

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
                    t->_process->_addressSpace)
                    ->_pml4Phys));
    }

    Scheduler::Context* Scheduler::context()
    {
        return &(getCPULocal()->_schedule);
    }
}