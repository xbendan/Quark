#include <quark/hal/interrupts.h>
#include <quark/os/main.h>

#include <mixins/meta/buf.h>
#include <mixins/std/panic.h>
#include <mixins/utils/arrays.h>
#include <quark/os/diagnostic/debug.h>
#include <quark/os/diagnostic/logging.h>

#include <drivers/comm/device.h>
#include <drivers/gfx/vesa/device_vga.h>

namespace Quark::System {
    using namespace Quark::System::Diagnostic;

    BootInfo                  launchConfig;
    Serial::SerialPortDevice  serial;
    VESA::VGATextOutputDevice vga;

    BootInfo& getLaunchConfiguration()
    {
        return launchConfig;
    }

    [[noreturn]]
    void SetupKernel()
    {
        Hal::Platform& platform = BootInfo::Platform;

        new (&serial) Serial::SerialPortDevice();
        new (&vga) VESA::VGATextOutputDevice();
        new (&_LoggerWriter) LoggerTextWriter(&serial, &vga);

        log("Setting up kernel..."s);

        log("Initializing architecture-specific features..."s);
        SetupArch();
        log("OK."s);

        log("Initializing virtual memory management...");
        AddressSpace* kernelAddressSpace = nullptr;
        if (platform._features.hasNot(Hal::Platform::AddressSpaceIsolation)) {
            log("VMM is not supported on this platform.");
        } else {
            kernelAddressSpace = BootInfo::MemoryInfo._addressSpace =
                InitVirtMemory().unwrap();
            log("OK."s);
        }

        log("Initializing physical memory management...");
        InitPhysMemory().unwrap();
        log("OK."s);

        // log("Initializing basic interrupts...");
        // Hal::SetupInterrupts().Unwrap();

        log("Creating kernel process...");
        Process::CreateKernelProcess(kernelAddressSpace).unwrap();

        log("Initializing device connectivity...");
        auto* devList = EnumerateInitialDevices().unwrap();

        for (auto& dev : *devList)
            Io::Device::Load(dev);

        log("Initializing task scheduler...");
        InitTasks().unwrap();

        asm volatile("sti");

        log("Done!");

        while (true)
            asm volatile("hlt; pause;");
    }
} // namespace Quark::System

namespace Std {
    using namespace Quark::System::Diagnostic;

    [[noreturn]] void panic(Error err)
    {
        panic(err.message());
    }

    [[noreturn]] void panic(const char* msg)
    {
        error("---- QUARK KERNEL PANIC ----");
        error$("Time:        {}", 0);
        error$("Description: {}", msg);
        error(" ");
        error("This is a fatal error, halting the system.");
        error("----------------------------");
        error("Stacktrace:");

        while (true)
            asm volatile("hlt; pause;");
    }
}