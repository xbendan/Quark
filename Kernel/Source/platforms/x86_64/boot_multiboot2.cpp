#include <boot_protocols/multiboot2.h>
#include <mixins/std/c++types.h>
#include <mixins/std/string.h>
#include <quark/init/boot_info.h>
#include <quark/memory/memory_info.h>
#include <quark/os/main.h>

// using namespace Quark::System;
using Quark::System::BootInfo;
using Quark::System::Hal::Platform;
using Quark::System::Memory::MemmapEntry;
using Quark::System::Memory::MemoryConfigTable;

// extern "C"
// {
//     __attribute__((used,
//                    section(".multiboot2"),
//                    aligned(8))) //
//     static volatile multiboot_header mbHeader = {
//         .magic         = MULTIBOOT2_HEADER_MAGIC,
//         .architecture  = MULTIBOOT_ARCHITECTURE_I386,
//         .header_length = sizeof(multiboot_header),
//         .checksum =
//             -(mbHeader.magic + mbHeader.architecture +
//             mbHeader.header_length),
//     };
// }

__attribute__((used)) //
static volatile multiboot_tag_load_base_addr* mbLoadBaseAddr;

extern "C" [[noreturn]]
void
kload_multiboot2(multiboot_info* _mbInfo)
{
    BootInfo::Platform = {
        "AMD64",
        "1.0",
        Platform::Type::Desktop,
        Platform::Bit64,
        Platform::AddressSpaceIsolation | Platform::ProcessContextSwitch,
    };

    multiboot_tag* tag = reinterpret_cast<multiboot_tag*>(_mbInfo->tags);
    // multiboot_tag_module* modules[16];

    auto* btMemoryTable = new (&BootInfo::MemoryInfo) MemoryConfigTable();

    while (tag->type != MULTIBOOT_TAG_TYPE_END &&
           reinterpret_cast<u64>(tag) < _mbInfo->total_size) {
        switch (tag->type) {
            case MULTIBOOT_TAG_TYPE_CMDLINE: {
                // auto* mbCmdline =
                //     reinterpret_cast<multiboot_tag_string*>(tag);
                break;
            }
            case MULTIBOOT_TAG_TYPE_BOOT_LOADER_NAME: {
                // auto* mbBootLoaderName =
                //     reinterpret_cast<multiboot_tag_string*>(tag);

                // Strings::Copy(mbBootLoaderName->string,
                //               BootInfo::BootloaderName);
                // TODO: Write bootloader name copy
                break;
            }
            case MULTIBOOT_TAG_TYPE_MODULE: {
                // auto* mbModule =
                //     reinterpret_cast<multiboot_tag_module*>(tag);
                break;
            }
            case MULTIBOOT_TAG_TYPE_BASIC_MEMINFO: {
                // auto* mbBasicMeminfo =
                //     reinterpret_cast<multiboot_tag_basic_meminfo*>(tag);
                break;
            }
            case MULTIBOOT_TAG_TYPE_BOOTDEV: {
                // auto* mbBootDev =
                //     reinterpret_cast<multiboot_tag_bootdev*>(tag);
                break;
            }
            case MULTIBOOT_TAG_TYPE_MMAP: {
                multiboot_tag_mmap* mbMemmap =
                    reinterpret_cast<multiboot_tag_mmap*>(tag);
                multiboot_memory_map_t* mbMemmapEntry = mbMemmap->entries;

                btMemoryTable->_totalSize += mbMemmap->size;

                for (int i = 0;
                     i < (mbMemmap->size - sizeof(multiboot_tag_mmap)) /
                             mbMemmap->entry_size &&
                     (reinterpret_cast<u64>(mbMemmapEntry) <
                      reinterpret_cast<u64>(tag) + mbMemmap->size);
                     i++) {
                    MemmapEntry& mapEntry = btMemoryTable->_addressRanges[0];

                    mapEntry.Range = {
                        mbMemmapEntry->addr,
                        mbMemmapEntry->len,
                    };

                    if (mbMemmapEntry->type != MULTIBOOT_MEMORY_RESERVED)
                        btMemoryTable->_limit =
                            max(btMemoryTable->_limit,
                                mbMemmapEntry->addr + mbMemmapEntry->len);

                    // clang-format off
                        switch (mbMemmapEntry->type) {
                            case MULTIBOOT_MEMORY_AVAILABLE: {
                                mapEntry.Type = MemmapEntry::Free;
                                btMemoryTable->_availableSize += mbMemmapEntry->len;
                                break;
                            }
                            case MULTIBOOT_MEMORY_RESERVED: /*
                                这些区间是没有被映射到任何地方，不能当作RAM来使用，
                                但是kernel可以决定将这些区间映射到其他地方，
                                比如PCI设备。*/ 
                                { mapEntry.Type = MemmapEntry::Reserved; break; }
                            case MULTIBOOT_MEMORY_ACPI_RECLAIMABLE: /*
                                映射到用来存放ACPI数据的RAM空间，操作系统应该将
                                ACPI Table读入到这个区间内。*/ 
                                { mapEntry.Type = MemmapEntry::AcpiReclaimable; break; }
                            case MULTIBOOT_MEMORY_NVS: /*
                                映射到用来存放ACPI数据的非易失性存储空间，
                                操作系统不能使用。*/ 
                                { mapEntry.Type = MemmapEntry::AcpiNvs; break; }
                            case MULTIBOOT_MEMORY_BADRAM: /*
                                内存物理损坏 */ 
                                { mapEntry.Type = MemmapEntry::BadMemory; break; }
                        }
                    // clang-format on

                    mbMemmapEntry = reinterpret_cast<multiboot_memory_map_t*>(
                        reinterpret_cast<u64>(mbMemmapEntry) +
                        mbMemmap->entry_size);
                }
                break;
            }
            case MULTIBOOT_TAG_TYPE_FRAMEBUFFER: {
                // auto* mbFramebuffer =
                //     reinterpret_cast<multiboot_tag_framebuffer*>(tag);
                break;
            }
            case MULTIBOOT_TAG_TYPE_LOAD_BASE_ADDR: {
                mbLoadBaseAddr =
                    reinterpret_cast<multiboot_tag_load_base_addr*>(tag);
                break;
            }
        }
        tag = reinterpret_cast<multiboot_tag*>(
            reinterpret_cast<u64>(tag) +
            ((tag->size + MULTIBOOT_TAG_ALIGN - 1) &
             ~(MULTIBOOT_TAG_ALIGN - 1)));
    }

    Quark::System::SetupKernel();

    while (true)
        asm volatile("hlt; pause;");
}