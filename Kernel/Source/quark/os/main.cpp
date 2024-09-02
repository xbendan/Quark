#include <quark/os/main.h>

#include <mixins/meta/buf.h>
#include <mixins/std/panic.h>
#include <quark/api/logging.h>

namespace Quark::System {
    OsBootConfig  bootInfo;
    Process*      kernelProcess;
    AddressSpace* kernelAddressSpace;

    OsBootConfig& getBootInfo()
    {
        return bootInfo;
    }

    Res<Process*> createKernelProcess()
    {
        static Inert<Process> kpInert;

        if (kernelProcess)
            return Error::InvalidState("Kernel process already exists.");

        return Ok(kpInert.ctor(0, "Kernel", kernelAddressSpace, 0, 0, 0));
    }

    [[noreturn]]
    void setupKernel(OsBootConfig* bootConfig)
    {
        Hal::PlatformDefinition& platform = bootConfig->_platform;
        log(u8"Setting up kernel...");

        log(u8"Initializing architecture-specific features...");
        setupArch(bootConfig);
        log(u8"OK.");

        log(u8"Initializing address space isolation (Virtual memory):\n");
        if (platform._features.hasNot(
                Hal::PlatformDefinition::AddressSpaceIsolation)) {
            log(u8"VMM is not supported on this platform.\n");
        } else
            kernelAddressSpace = initVirtMemory().unwrap();

        log(u8"Initializing memory management...\n");
        initPhysMemory().unwrap();

        log(u8"Creating kernel process...\n");
        kernelProcess = createKernelProcess().unwrap();

        log(u8"Initializing device connectivity...\n");
        setupDevices().unwrap();

        log(u8"Initializing task scheduler...\n");
        initTasks().unwrap();

        log(u8"Done!");

        while (true)
            asm volatile("hlt; pause;");
    }
} // namespace Quark::System

namespace Std {
    [[noreturn]] void panic(Error err)
    {
        while (true)
            asm volatile("hlt; pause;");
    }

    [[noreturn]] void panic(String<> msg)
    {
        // TODO: Implement panic
        log(u8"---- QUARK KERNEL PANIC ----");
        log(u8"Time:        {}", 0);
        log(u8"Description: {}", msg);
        log(u8" ");
        log(u8"This is a fatal error, halting the system.");
        log(u8"Stacktrace:");
        log(u8"----------------------------");

        while (true)
            asm volatile("hlt; pause;");
    }
}