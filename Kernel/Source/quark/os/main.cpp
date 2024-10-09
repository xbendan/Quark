#include <quark/os/main.h>

#include <mixins/meta/buf.h>
#include <mixins/std/panic.h>
#include <quark/api/logging.h>

namespace Quark::System {
    LaunchConfiguration launchConfig;

    LaunchConfiguration& getLaunchConfiguration()
    {
        return launchConfig;
    }

    [[noreturn]]
    void SetupKernel(LaunchConfiguration* conf)
    {
        Hal::Platform& platform = conf->_platform;
        log(u8"Setting up kernel...");

        log(u8"Initializing architecture-specific features...");
        SetupArch(conf);
        log(u8"OK.");

        log(u8"Initializing address space isolation (Virtual memory):\n");
        AddressSpace* kernelAddressSpace = nullptr;
        if (platform._features.hasNot(Hal::Platform::AddressSpaceIsolation)) {
            log(u8"VMM is not supported on this platform.\n");
        } else
            kernelAddressSpace = InitVirtMemory().Unwrap();

        log(u8"Initializing memory management...\n");
        InitPhysMemory().Unwrap();

        log(u8"Creating kernel process...\n");
        Process::CreateKernelProcess(kernelAddressSpace).Unwrap();

        log(u8"Initializing device connectivity...\n");
        EnumerateInitialDevices().Unwrap()->ForEach(
            [](Io::Device* device) { device->OnInitialize().Unwrap(); });

        log(u8"Initializing task scheduler...\n");
        InitTasks().Unwrap();

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

    [[noreturn]] void panic(string msg)
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