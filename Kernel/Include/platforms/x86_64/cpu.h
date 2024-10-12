#define CPUID_ECX_SSE3 (1 << 0)
#define CPUID_ECX_PCLMUL (1 << 1)
#define CPUID_ECX_DTES64 (1 << 2)
#define CPUID_ECX_MONITOR (1 << 3)
#define CPUID_ECX_DS_CPL (1 << 4)
#define CPUID_ECX_VMX (1 << 5)
#define CPUID_ECX_SMX (1 << 6)
#define CPUID_ECX_EST (1 << 7)
#define CPUID_ECX_TM2 (1 << 8)
#define CPUID_ECX_SSSE3 (1 << 9)
#define CPUID_ECX_CID (1 << 10)
#define CPUID_ECX_FMA (1 << 12)
#define CPUID_ECX_CX16 (1 << 13)
#define CPUID_ECX_ETPRD (1 << 14)
#define CPUID_ECX_PDCM (1 << 15)
#define CPUID_ECX_PCIDE (1 << 17)
#define CPUID_ECX_DCA (1 << 18)
#define CPUID_ECX_SSE4_1 (1 << 19)
#define CPUID_ECX_SSE4_2 (1 << 20)
#define CPUID_ECX_x2APIC (1 << 21)
#define CPUID_ECX_MOVBE (1 << 22)
#define CPUID_ECX_POPCNT (1 << 23)
#define CPUID_ECX_AES (1 << 25)
#define CPUID_ECX_XSAVE (1 << 26)
#define CPUID_ECX_OSXSAVE (1 << 27)
#define CPUID_ECX_AVX (1 << 28)

#define CPUID_EDX_FPU (1 << 0)
#define CPUID_EDX_VME (1 << 1)
#define CPUID_EDX_DE (1 << 2)
#define CPUID_EDX_PSE (1 << 3)
#define CPUID_EDX_TSC (1 << 4)
#define CPUID_EDX_MSR (1 << 5)
#define CPUID_EDX_PAE (1 << 6)
#define CPUID_EDX_MCE (1 << 7)
#define CPUID_EDX_CX8 (1 << 8)
#define CPUID_EDX_APIC (1 << 9)
#define CPUID_EDX_SEP (1 << 11)
#define CPUID_EDX_MTRR (1 << 12)
#define CPUID_EDX_PGE (1 << 13)
#define CPUID_EDX_MCA (1 << 14)
#define CPUID_EDX_CMOV (1 << 15)
#define CPUID_EDX_PAT (1 << 16)
#define CPUID_EDX_PSE36 (1 << 17)
#define CPUID_EDX_PSN (1 << 18)
#define CPUID_EDX_CLF (1 << 19)
#define CPUID_EDX_DTES (1 << 21)
#define CPUID_EDX_ACPI (1 << 22)
#define CPUID_EDX_MMX (1 << 23)
#define CPUID_EDX_FXSR (1 << 24)
#define CPUID_EDX_SSE (1 << 25)
#define CPUID_EDX_SSE2 (1 << 26)
#define CPUID_EDX_SS (1 << 27)
#define CPUID_EDX_HTT (1 << 28)
#define CPUID_EDX_TM1 (1 << 29)
#define CPUID_EDX_IA64 (1 << 30)
#define CPUID_EDX_PBE (1 << 31)

#define MSR_APIC_BASE 0x1b
#define MSR_EFER 0xC0000080
#define MSR_STAR 0xC0000081
#define MSR_LSTAR 0xC0000082
#define MSR_CSTAR 0xC0000083
#define MSR_SYSCALL_MASK 0xC0000084
#define MSR_FS_BASE 0xC0000100
#define MSR_USER_GS_BASE 0xC0000101
#define MSR_KERN_GS_BASE 0xC0000102
#define MSR_TSC_AUX 0xC0000103

#define CR0_PROTECTION_MODE (1 << 0)
#define CR0_MONITOR_COPROCESSOR (1 << 1)
#define CR0_EMULATE_COPROCESSOR (1 << 2)
#define CR0_TASK_SWITCHED (1 << 3)
#define CR0_EXTENSION_TYPE (1 << 4)
#define CR0_NUMERIC_ERROR (1 << 5)
#define CR0_WRITE_PROTECT (1 << 16)
#define CR0_ALIGNMENT_MASK (1 << 18)
#define CR0_NOT_WRITE_THROUGH (1 << 29)
#define CR0_CACHE_DISABLE (1 << 30)
#define CR0_PAGING (1 << 31)

#define CR4_V8086_MODE (1 << 0)
#define CR4_PMODE_VIRT_INT (1 << 1)
#define CR4_TIME_STAMP_DISABLE (1 << 2)
#define CR4_DEBUG_EXT (1 << 3)
#define CR4_PAGE_SIZE_EXT (1 << 4)
#define CR4_PHYS_ADDR_EXT (1 << 5)
#define CR4_MACHINE_CHECK (1 << 6)
#define CR4_PAGE_GLOBAL_ENABLE (1 << 7)
#define CR4_PERFMONITOR (1 << 8)
#define CR4_OSFXSR (1 << 9)
#define CR4_OSXMMEXCPT (1 << 10)
#define CR4_USER_MODE_INSTRUCTION_PREVENTION (1 << 11)
#define CR4_LINEAR_ADDR_57BIT (1 << 12)
#define CR4_VIRT_MACHINE_EXT (1 << 13)
#define CR4_SAFER_MODE_EXT (1 << 14)
#define CR4_FSGSBASE                                                           \
    (1 << 16) // Enables the instructions RDFSBASE, RDGSBASE, WRFSBASE, and
              // WRGSBASE
#define CR4_PCID (1 << 17)    // Enables the process-context identifiers (PCIDs)
#define CR4_OSXSAVE (1 << 18) // XSAVE and Processor Extended States Enable Bit
#define CR4_SUPERVISOR_EXEC_PROTECTION (1 << 20)
#define CR4_SUPERVISOR_ACCESS_PREVENTION (1 << 21)
#define CR4_PROTECTION_KEY_ENABLE (1 << 22)
#define CR4_CONTROL_FLOW_ENFORCEMENT (1 << 23)
#define CR4_PROTECTION_KEY_SUPERVISOR (1 << 24)

#include <mixins/std/c++types.h>
#include <mixins/utils/array.h>
#include <platforms/x86_64/sched.h>

namespace Quark::System::Platform::X64 {
    // enum ModelSpecReg : u32 {
    //     APIC_BASE    = 0x1b,
    //     EFER         = 0xC0000080,
    //     STAR         = 0xC0000081,
    //     LSTAR        = 0xC0000082,
    //     CSTAR        = 0xC0000083,
    //     SYSCALL_MASK = 0xC0000084,
    //     FS_BASE      = 0xC0000100,
    //     USER_GS_BASE = 0xC0000101,
    //     KERN_GS_BASE = 0xC0000102,
    //     TSC_AUX      = 0xC0000103,
    // };

    union CPUID
    {
        enum class Vendor
        {
            Unknown,
            Intel,
            AMD,
            Cyrix,
            VIA,
            Transmeta,
            Centaur,
            NexGen,
            Rise,
            SiS,
            UMC,
            NSC,
            Hygon,
            Zhaoxin,
            HEDT,
        };

        struct
        {
            u32 _eax;
            u32 _ebx;
            u32 _ecx;
            u32 _edx;
        };

        Array<char[16]> _ven;

        CPUID(u32 leaf, u32 subleaf)
        {
            u32 eax, ebx, ecx, edx;
            asm volatile("cpuid"
                         : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx)
                         : "a"(leaf), "c"(subleaf));
            // assert leaf <= max leaf
            if (leaf == 0) {
                for (int i = 0; i < 4; i++) {
                    _ven[i * 4 + 0] = ebx >> (i * 8);
                    _ven[i * 4 + 1] = ecx >> (i * 8);
                    _ven[i * 4 + 2] = edx >> (i * 8);
                }
                _edx = 0;
            } else {
                _eax = eax;
                _ebx = ebx;
                _ecx = ecx;
                _edx = edx;
            }
        }
    };

    static inline u64 rdmsr(u32 reg)
    {
        u32 low, high;
        asm volatile("rdmsr" : "=a"(low), "=d"(high) : "c"(reg));
        return ((u64)high << 32) | low;
    }

    static inline void wrmsr(u32 reg, u64 value)
    {
        u32 low  = value & 0xFFFFFFFF;
        u32 high = value >> 32;
        asm volatile("wrmsr" : : "c"(reg), "a"(low), "d"(high));
    }

    static inline u32 rdcr0()
    {
        u32 cr0;
        asm volatile("mov %%cr0, %0" : "=r"(cr0));
        return cr0;
    }

    static inline void wrcr0(u32 cr0)
    {
        asm volatile("mov %0, %%cr0" : : "r"(cr0));
    }

    static inline u32 rdcr2()
    {
        u32 cr2;
        asm volatile("mov %%cr2, %0" : "=r"(cr2));
        return cr2;
    }

    static inline u32 rdcr3()
    {
        u32 cr3;
        asm volatile("mov %%cr3, %0" : "=r"(cr3));
        return cr3;
    }

    static inline void wrcr3(u32 cr3)
    {
        asm volatile("mov %0, %%cr3" : : "r"(cr3));
    }

    static inline u32 rdcr4()
    {
        u32 cr4;
        asm volatile("mov %%cr4, %0" : "=r"(cr4));
        return cr4;
    }

    static inline void wrcr4(u32 cr4)
    {
        asm volatile("mov %0, %%cr4" : : "r"(cr4));
    }

    template <u8 N>
        requires(N >= 0 && N <= 4)
    struct CR
    {
        void operator=(u32 val)
        {
            switch (N) {
                case 0:
                    wrcr0(val);
                    break;
                case 2:
                    // wrcr2(val);
                    break;
                case 3:
                    wrcr3(val);
                    break;
                case 4:
                    wrcr4(val);
                    break;
                default:
                    break;
            }
        }

        u32 operator()()
        {
            switch (N) {
                case 0:
                    return rdcr0();
                case 2:
                    return rdcr2();
                case 3:
                    return rdcr3();
                case 4:
                    return rdcr4();
                default:
                    return 0;
            }
        }

        void operator|=(u32 val) { *this = *this() | val; }
        void operator&=(u32 val) { *this = *this() & val; }
        void operator^=(u32 val) { *this = *this() ^ val; }

        CR& operator+=(u32 val)
        {
            *this = *this() | val;
            return *this;
        }

        CR& operator-=(u32 val)
        {
            *this = *this() & ~val;
            return *this;
        }
    };

    static inline void swapgs()
    {
        asm volatile("swapgs");
    }

    static inline void SetCPULocal(CPULocalDevice* cpu)
    {
        asm volatile("wrmsr" ::"a"((u64)cpu & 0xffffffff) /* Value low */,
                     "d"(((u64)cpu >> 32) & 0xffffffff) /* Value high */,
                     "c"(MSR_KERN_GS_BASE) /* Set Kernel GS Base */);
        asm volatile("wrmsr" ::"a"((u64)cpu & 0xffffffff) /* Value low */,
                     "d"(((u64)cpu >> 32) & 0xffffffff) /* Value high */,
                     "c"(MSR_USER_GS_BASE) /* Set Kernel GS Base */);
    }

    static inline CPULocalDevice* GetCPULocal()
    {
        CPULocalDevice* cpu;
        asm volatile("swapgs; movq %%gs:0, %0; swapgs;" : "=r"(cpu));
        return cpu;
    }

    static inline bool CheckInterrupts()
    {
        volatile u64 flags;
        asm volatile("pushfq;"
                     "pop %0;"
                     : "=rm"(flags)::"memory", "cc");
        return flags & 0x200;
    }
} // namespace Quark::System::Platform::X64