#include <quark/os/main.h>

#include <mixins/meta/buf.h>
#include <mixins/std/panic.h>
#include <quark/os/logging.h>

#include <drivers/comm/device.h>

namespace Quark::System {
    using namespace Quark::System::Diagnostic;

    BootInfo                 launchConfig;
    Serial::SerialPortDevice serial;

    BootInfo& getLaunchConfiguration()
    {
        return launchConfig;
    }

    [[noreturn]]
    void SetupKernel()
    {
        Hal::Platform& platform = BootInfo::Platform;

        new (&serial) Serial::SerialPortDevice();
        log(u8"Setting up kernel...");

        log(u8"Initializing architecture-specific features...");
        SetupArch();
        log(u8"OK.");

        log(u8"Initializing address space isolation (Virtual memory):\n");
        AddressSpace* kernelAddressSpace = nullptr;
        if (platform._features.hasNot(Hal::Platform::AddressSpaceIsolation)) {
            log(u8"VMM is not supported on this platform.\n");
        } else
            kernelAddressSpace = BootInfo::MemoryInfo._addressSpace =
                InitVirtMemory().Unwrap();

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
    using namespace Quark::System::Diagnostic;

    [[noreturn]] void ThrowError(Error err)
    {
        while (true)
            asm volatile("hlt; pause;");
    }

    [[noreturn]] void SystemPanic(string msg)
    {
        // TODO: Implement panic
        error(u8"---- QUARK KERNEL PANIC ----");
        log(LogLevel::Error, u8"Time:        {}", 0);
        log(LogLevel::Error, u8"Description: {}", msg);
        log(LogLevel::Error, u8" ");
        log(LogLevel::Error, u8"This is a fatal error, halting the system.");
        log(LogLevel::Error, u8"Stacktrace:");
        log(LogLevel::Error, u8"----------------------------");

        while (true)
            asm volatile("hlt; pause;");
    }
}