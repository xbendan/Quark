#include <boot_protocols/limine.h>

extern "C"
{
    __attribute__((used,
                   section(".requests"))) //
    static volatile LIMINE_BASE_REVISION(2);

    __attribute__((used,
                   section(".requests"))) //
    static volatile limine_framebuffer_request lmReqFramebuffer;

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
{}