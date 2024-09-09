#include <boot_protocols/stivale2.h>
#include <mixins/meta/inert.h>
#include <quark/init/boot_info.h>
#include <quark/os/main.h>

using namespace Quark::System::Memory;
// static Array<OsMemRangeInfo[256]> memmaps;

extern "C" [[noreturn]]
void
kload_st2(stivale2_struct* stInfo)
{
    if (!stInfo) {
        // TODO: Panic
        while (true)
            asm volatile("hlt");
    }

    OsBootConfig& bootConfig = Quark::System::getBootInfo();

    bootConfig._platform = {
        "AMD64",
        "1.0",
        64,
        Quark::System::Hal::PlatformDefinition::AddressSpaceIsolation |
            Quark::System::Hal::PlatformDefinition::ProcessContextSwitch,
    };

    stivale2_tag* tag = reinterpret_cast<stivale2_tag*>(stInfo->tags);
    while (tag) {
        switch (tag->identifier) {
            case STIVALE2_STRUCT_TAG_CMDLINE_ID: {
                break;
            }
            case STIVALE2_STRUCT_TAG_MEMMAP_ID: {
                stivale2_struct_tag_memmap* tagMemmap =
                    reinterpret_cast<stivale2_struct_tag_memmap*>(tag);

                OsMemoryConfig& osMemInfo = bootConfig._memory;
                for (u64 i = 0; i < tagMemmap->entries; i++) {
                    stivale2_mmap_entry* entry = &tagMemmap->memmap[i];
                    OsMemRangeInfo&      osMemRangeInfo =
                        osMemInfo._addressRanges[i];

                    osMemInfo._totalSize += entry->length;
                    osMemRangeInfo._value =
                        AddressRange(entry->base, entry->length);
                    switch (entry->type) {
                        case STIVALE2_MMAP_USABLE: {
                            osMemInfo._availableSize += entry->length;
                            osMemRangeInfo._type =
                                OsMemRangeType::MEM_RANGE_TYPE_FREE;
                            break;
                        }
                        case STIVALE2_MMAP_KERNEL_AND_MODULES: {
                            osMemRangeInfo._type =
                                OsMemRangeType::MEM_RANGE_TYPE_KERNEL;
                            break;
                        }
                        case STIVALE2_MMAP_ACPI_RECLAIMABLE: {
                            osMemRangeInfo._type =
                                OsMemRangeType::MEM_RANGE_TYPE_ACPI_RECLAIMABLE;
                            break;
                        }
                        case STIVALE2_MMAP_BOOTLOADER_RECLAIMABLE: {
                            osMemRangeInfo._type =
                                OsMemRangeType::MEM_RANGE_TYPE_FREE;
                            break;
                        }
                        case STIVALE2_MMAP_BAD_MEMORY: {
                            osMemRangeInfo._type =
                                OsMemRangeType::MEM_RANGE_TYPE_BAD;
                            break;
                        }
                        case STIVALE2_MMAP_ACPI_NVS: {
                            osMemRangeInfo._type =
                                OsMemRangeType::MEM_RANGE_TYPE_ACPI_NVS;
                            break;
                        }
                        case STIVALE2_MMAP_RESERVED: {
                            osMemRangeInfo._type =
                                OsMemRangeType::MEM_RANGE_TYPE_RESERVED;
                            break;
                        }
                    }
                }
                break;
            }
            case STIVALE2_STRUCT_TAG_FRAMEBUFFER_ID: {
                stivale2_struct_tag_framebuffer* tagFb =
                    reinterpret_cast<stivale2_struct_tag_framebuffer*>(tag);
                bootConfig._gfx = {
                    ._width   = tagFb->framebuffer_width,
                    ._height  = tagFb->framebuffer_height,
                    ._address = tagFb->framebuffer_addr,
                    ._depth   = tagFb->framebuffer_bpp,
                    ._pitch   = tagFb->framebuffer_pitch,
                };
                break;
            }
            case STIVALE2_STRUCT_TAG_MODULES_ID: {
                break;
            }
        }
    }
    Quark::System::setupKernel(&bootConfig);
}