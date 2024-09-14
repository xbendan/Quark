#include <boot_protocols/stivale2.h>
#include <mixins/meta/inert.h>
#include <quark/init/boot_info.h>
#include <quark/os/main.h>

using namespace Quark::System::Memory;
using Quark::System::LaunchConfiguration;
using Quark::System::Graphics::LinearFramebufferDevice;
using Quark::System::Hal::Platform;

extern "C" [[noreturn]]
void
kload_st2(stivale2_struct* stInfo)
{
    if (!stInfo) {
        // TODO: Panic
        while (true)
            asm volatile("hlt");
    }

    LaunchConfiguration& conf = Quark::System::getLaunchConfiguration();

    conf._platform = {
        "AMD64",
        "1.0",
        Platform::Type::Desktop,
        Platform::Bit64,
        Platform::AddressSpaceIsolation | Platform::ProcessContextSwitch,
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

                MemoryConfiguration& confMemory = conf._memory;
                for (u64 i = 0; i < tagMemmap->entries; i++) {
                    stivale2_mmap_entry* entry = &tagMemmap->memmap[i];
                    MemmapEntry& mapEntry      = confMemory._addressRanges[i];

                    confMemory._totalSize += entry->length;
                    mapEntry._range = AddressRange(entry->base, entry->length);
                    switch (entry->type) {
                        case STIVALE2_MMAP_USABLE: {
                            confMemory._availableSize += entry->length;
                            mapEntry._type = MemmapEntry::Type::Free;
                            break;
                        }
                        case STIVALE2_MMAP_KERNEL_AND_MODULES: {
                            mapEntry._type = MemmapEntry::Type::Kernel;
                            break;
                        }
                        case STIVALE2_MMAP_ACPI_RECLAIMABLE: {
                            mapEntry._type = MemmapEntry::Type::AcpiReclaimable;
                            break;
                        }
                        case STIVALE2_MMAP_BOOTLOADER_RECLAIMABLE: {
                            confMemory._availableSize += entry->length;
                            mapEntry._type = MemmapEntry::Type::Reclaimable;
                            break;
                        }
                        case STIVALE2_MMAP_BAD_MEMORY: {
                            mapEntry._type = MemmapEntry::Type::BadMemory;
                            break;
                        }
                        case STIVALE2_MMAP_ACPI_NVS: {
                            mapEntry._type = MemmapEntry::Type::AcpiNvs;
                            break;
                        }
                        case STIVALE2_MMAP_RESERVED: {
                            mapEntry._type = MemmapEntry::Type::Reserved;
                            break;
                        }
                    }
                }
                break;
            }
            case STIVALE2_STRUCT_TAG_FRAMEBUFFER_ID: {
                // stivale2_struct_tag_framebuffer* tagFb =
                //     reinterpret_cast<stivale2_struct_tag_framebuffer*>(tag);
                // conf._framebuffer =
                //     Opt<LinearFramebufferDevice>(tagFb->framebuffer_addr,
                //                                  (u32)tagFb->framebuffer_width,
                //                                  (u32)tagFb->framebuffer_height,
                //                                  tagFb->framebuffer_pitch,
                //                                  tagFb->framebuffer_bpp);
                break;
            }
            case STIVALE2_STRUCT_TAG_MODULES_ID: {
                break;
            }
        }
    }
    Quark::System::setupKernel(&conf);
}