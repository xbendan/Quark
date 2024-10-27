#include <boot_protocols/limine.h>
#include <mixins/utils/array.h>
#include <quark/gfx/framebuffer.h>
#include <quark/hal/platform.h>
#include <quark/init/boot_info.h>
#include <quark/os/main.h>

using Quark::System::BootInfo;
using Quark::System::Graphics::LinearFramebufferDevice;
using Quark::System::Hal::Platform;
using Quark::System::Memory::AddressRange;
using Quark::System::Memory::MemmapEntry;
using Quark::System::Memory::MemoryConfigTable;

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

    // __attribute__((used,
    //                section(".requests"))) //
    // static volatile limine_hhdm_request lmReqHhdm{
    //     .id       = LIMINE_HHDM_REQUEST,
    //     .revision = 0,
    //     .response = {},
    // };

    __attribute__((used,
                   section(".requests"))) //
    static volatile limine_kernel_address_request lmReqKernelAddress{
        .id       = LIMINE_KERNEL_ADDRESS_REQUEST,
        .revision = 0,
        .response = {},
    };

    // __attribute__((used,
    //                section(".requests_start_marker"))) //
    // static volatile LIMINE_REQUESTS_START_MARKER;

    // __attribute__((used,
    //                section(".requests_end_marker"))) //
    // static volatile LIMINE_REQUESTS_END_MARKER;

    [[noreturn]]
    void kload_limine(void)
    {
        BootInfo::Platform = {
            "AMD64",
            "1.0",
            Platform::Type::Desktop,
            Platform::Bit64,
            Platform::AddressSpaceIsolation | Platform::ProcessContextSwitch,
        };

        BootInfo::BootTime = lmReqBootTime.response->boot_time;

        // Handle memory map
        auto& mem = *(new (&BootInfo::MemoryInfo) MemoryConfigTable());

        limine_memmap_response* lmRespMemmap = lmReqMemmap.response;
        for (int i = 0; i < lmRespMemmap->entry_count; i++) {
            MemmapEntry&         mapEntry = mem._addressRanges[i];
            limine_memmap_entry* entry    = lmRespMemmap->entries[i];

            mem._totalSize += entry->length;
            mapEntry.Range = AddressRange(entry->base, entry->length);
            switch (entry->type) {
                case LIMINE_MEMMAP_USABLE: {
                    mem._availableSize += entry->length;
                    mapEntry.Type = MemmapEntry::Type::Free;
                    break;
                }
                case LIMINE_MEMMAP_RESERVED: /*
                    这些区间是没有被映射到任何地方，不能当作RAM来使用，
                    但是kernel可以决定将这些区间映射到其他地方，
                    比如PCI设备。 */
                {
                    mapEntry.Type = MemmapEntry::Type::Reserved;
                    break;
                }
                case LIMINE_MEMMAP_KERNEL_AND_MODULES: {
                    mapEntry.Type = MemmapEntry::Type::Kernel;
                    break;
                }
                case LIMINE_MEMMAP_ACPI_NVS: /*
                    映射到用来存放ACPI数据的非易失性存储空间，
                    操作系统不能使用。*/
                {
                    mapEntry.Type = MemmapEntry::Type::AcpiNvs;
                    break;
                }
                case LIMINE_MEMMAP_ACPI_RECLAIMABLE: /*
                    映射到用来存放ACPI数据的RAM空间，操作系统应该将
                    ACPI Table读入到这个区间内。*/
                {
                    mapEntry.Type = MemmapEntry::Type::AcpiReclaimable;
                    break;
                }
                case LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE: {
                    mem._availableSize += entry->length;
                    mapEntry.Type = MemmapEntry::Type::Reclaimable;
                    break;
                }
                case LIMINE_MEMMAP_BAD_MEMORY: {
                    mapEntry.Type = MemmapEntry::Type::BadMemory;
                    break;
                }
                case LIMINE_MEMMAP_FRAMEBUFFER: {
                    // LinearFramebufferDevice* fb = new
                    // LinearFramebufferDevice(); fb->init(entry->base,
                    // entry->length, 0, 0, 0); conf._graphics._framebuffer =
                    // fb;
                    mapEntry.Type = MemmapEntry::Type::Data;
                    break;
                }
            }

            if (entry->type != LIMINE_MEMMAP_RESERVED) {
                mem._limit = max(mem._limit, entry->base + entry->length);
            }
        }

        limine_kernel_address_response* lmRespKernelAddress =
            lmReqKernelAddress.response;
        mem._kPhysOffset = lmRespKernelAddress->physical_base;
        mem._kVirtOffset = lmRespKernelAddress->virtual_base;

        Quark::System::SetupKernel();

        while (true)
            asm volatile("hlt; pause;");
    }

    __attribute__((used,
                   section(".requests"))) //
    static volatile limine_entry_point_request lmReqEntryPoint{
        .id       = LIMINE_ENTRY_POINT_REQUEST,
        .revision = 0,
        .response = {},
        .entry    = kload_limine,
    };
}
