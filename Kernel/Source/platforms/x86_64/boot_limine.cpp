#include <boot_protocols/limine.h>
#include <mixins/utils/array.h>
#include <quark/gfx/framebuffer.h>
#include <quark/hal/platform.h>
#include <quark/init/boot_info.h>
#include <quark/os/main.h>

using Quark::System::LaunchConfiguration;
using Quark::System::Graphics::LinearFramebufferDevice;
using Quark::System::Hal::Platform;
using Quark::System::Memory::AddressRange;
using Quark::System::Memory::MemmapEntry;
using Quark::System::Memory::MemoryConfiguration;

extern "C"
{
    __attribute__((used,
                   section(".requests"))) //
    static volatile LIMINE_BASE_REVISION(2);

    // __attribute__((used,
    //                section(".requests"))) //
    // static volatile limine_framebuffer_request lmReqFramebuffer;

    __attribute__((used,
                   section(".requests"))) //
    static volatile limine_memmap_request lmReqMemmap{
        .id       = LIMINE_MEMMAP_REQUEST,
        .revision = 0,
        .response = {}
    };

    __attribute__((used,
                   section(".requests"))) //
    static volatile limine_boot_time_request lmReqBootTime{
        .id       = LIMINE_BOOT_TIME_REQUEST,
        .revision = 0,
        .response = {}
    };

    __attribute__((used,
                   section(".requests"))) //
    static volatile limine_bootloader_info_request lmReqBootInfo{
        .id       = LIMINE_BOOTLOADER_INFO_REQUEST,
        .revision = 0,
        .response = {}
    };

    __attribute__((used,
                   section(".requests"))) //
    static volatile limine_stack_size_request lmReqStackSize{
        .id         = LIMINE_STACK_SIZE_REQUEST,
        .revision   = 0,
        .response   = {},
        .stack_size = 0x10000,
    };

    // If using EFI, the following requests are also required:
    // __attribute__((used,
    //                section(".requests"))) //
    // static volatile limine_efi_system_table_request lmReqEfiSystemTable{
    //     .id       = LIMINE_EFI_SYSTEM_TABLE_REQUEST,
    //     .revision = 0
    // };

    __attribute__((used,
                   section(".requests_start_marker"))) //
    static volatile LIMINE_REQUESTS_START_MARKER;

    __attribute__((used,
                   section(".requests_end_marker"))) //
    static volatile LIMINE_REQUESTS_END_MARKER;
}

extern "C" [[noreturn]]
void
kload_limine(void)
{
    LaunchConfiguration& conf = Quark::System::getLaunchConfiguration();

    conf._platform = {
        "AMD64",
        "1.0",
        Platform::Type::Desktop,
        Platform::Bit64,
        Platform::AddressSpaceIsolation | Platform::ProcessContextSwitch,
    };

    conf._bootTime = lmReqBootTime.response->boot_time;

    // Handle memory map
    MemoryConfiguration&    confMemory   = conf._memory;
    limine_memmap_response* lmRespMemmap = lmReqMemmap.response;
    for (int i = 0; i < lmRespMemmap->entry_count; i++) {
        MemmapEntry&         mapEntry = confMemory._addressRanges[i];
        limine_memmap_entry* entry    = lmRespMemmap->entries[i];

        mapEntry._range = AddressRange(entry->base, entry->length);
        switch (entry->type) {
            case LIMINE_MEMMAP_USABLE: {
                confMemory._availableSize += entry->length;
                mapEntry._type = MemmapEntry::Type::Free;
                break;
            }
            case LIMINE_MEMMAP_KERNEL_AND_MODULES: {
                mapEntry._type = MemmapEntry::Type::Kernel;
                break;
            }
            case LIMINE_MEMMAP_ACPI_RECLAIMABLE: {
                mapEntry._type = MemmapEntry::Type::AcpiReclaimable;
                break;
            }
            case LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE: {
                confMemory._availableSize += entry->length;
                mapEntry._type = MemmapEntry::Type::Reclaimable;
                break;
            }
            case LIMINE_MEMMAP_BAD_MEMORY: {
                mapEntry._type = MemmapEntry::Type::BadMemory;
                break;
            }
            case LIMINE_MEMMAP_FRAMEBUFFER: {
                // LinearFramebufferDevice* fb = new LinearFramebufferDevice();
                // fb->init(entry->base, entry->length, 0, 0, 0);
                // conf._graphics._framebuffer = fb;
                mapEntry._type = MemmapEntry::Type::Data;
                break;
            }
        }
    }

    Quark::System::setupKernel(&conf);

    while (true)
        asm volatile("hlt; pause;");
}