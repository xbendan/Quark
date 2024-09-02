#include <quark/os/main.h>

#include <mixins/meta/buf.h>
#include <mixins/std/panic.h>
#include <quark/api/logging.h>

namespace Quark {
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
        log("Setting up kernel...");

        log("Initializing architecture-specific features...");
        setupArch(bootConfig);
        log("OK.");

        log("Initializing address space isolation (Virtual memory):\n");
        if (platform._features.hasNot(
                Hal::PlatformDefinition::AddressSpaceIsolation)) {
            log("VMM is not supported on this platform.\n");
        } else
            kernelAddressSpace = initVirtMemory().unwrap();

        log("Initializing memory management...\n");
        initPhysMemory().unwrap();

        log("Creating kernel process...\n");
        kernelProcess = createKernelProcess().unwrap();

        log("Initializing device connectivity...\n");
        setupDevices().unwrap();

        log("Initializing task scheduler...\n");
        initTasks().unwrap();

        log("Done!");

        while (true)
            asm volatile("hlt; pause;");
    }
} // namespace Quark

namespace Std {
    [[noreturn]] void panic(Error err)
    {
        while (true)
            asm volatile("hlt; pause;");
    }

    [[noreturn]] void panic(String<> msg)
    {
        // TODO: Implement panic
        log("---- QUARK KERNEL PANIC ----");
        log("Time:        {}", 0);
        log("Description: {}", msg);
        log(" ");
        log("This is a fatal error, halting the system.");
        log("Stacktrace:");
        log("----------------------------");

        while (true)
            asm volatile("hlt; pause;");
    }
}