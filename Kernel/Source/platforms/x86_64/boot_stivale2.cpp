#include <boot_protocols/stivale2.h>
#include <mixins/meta/inert.h>
#include <quark/init/boot_info.h>
#include <quark/os/main.h>

#include <quark/memory/address_space.h>

using namespace Quark::System::Memory;
using Quark::System::BootInfo;
using Quark::System::Graphics::LinearFramebufferDevice;
using Quark::System::Hal::Platform;

extern "C" [[noreturn]]
void
kload_stivale2(stivale2_struct* stInfo)
{
    if (!stInfo) {
        // TODO: Panic
        while (true)
            asm volatile("hlt");
    }

    stInfo = (stivale2_struct*)((u64)stInfo + KERNEL_VIRTUAL_BASE);

    BootInfo::Platform = {
        "AMD64",
        "1.0",
        Platform::Type::Desktop,
        Platform::Bit64,
        Platform::AddressSpaceIsolation | Platform::ProcessContextSwitch,
    };

    u64 pTag = stInfo->tags;

    while (pTag) {
        stivale2_tag* tag =
            reinterpret_cast<stivale2_tag*>(pTag + KERNEL_VIRTUAL_BASE);

        switch (tag->identifier) {
            case STIVALE2_STRUCT_TAG_CMDLINE_ID: {
                break;
            }
            case STIVALE2_STRUCT_TAG_MEMMAP_ID: {
                stivale2_struct_tag_memmap* tagMemmap =
                    reinterpret_cast<stivale2_struct_tag_memmap*>(tag);

                auto& mem = *(new (&BootInfo::MemoryInfo) MemoryConfigTable());
                for (u64 i = 0; i < tagMemmap->entries; i++) {
                    stivale2_mmap_entry* entry    = &tagMemmap->memmap[i];
                    MemmapEntry&         mapEntry = mem._addressRanges[i];

                    mem._totalSize += entry->length;
                    mapEntry.Range = AddressRange(entry->base, entry->length);

                    switch (entry->type) {
                        case STIVALE2_MMAP_USABLE: {
                            mem._availableSize += entry->length;
                            mapEntry.Type = MemmapEntry::Type::Free;
                            break;
                        }
                        case STIVALE2_MMAP_KERNEL_AND_MODULES: {
                            mapEntry.Type = MemmapEntry::Type::Kernel;
                            break;
                        }
                        case STIVALE2_MMAP_ACPI_RECLAIMABLE: {
                            mapEntry.Type = MemmapEntry::Type::AcpiReclaimable;
                            break;
                        }
                        case STIVALE2_MMAP_BOOTLOADER_RECLAIMABLE: {
                            mem._availableSize += entry->length;
                            mapEntry.Type = MemmapEntry::Type::Reclaimable;
                            break;
                        }
                        case STIVALE2_MMAP_BAD_MEMORY: {
                            mapEntry.Type = MemmapEntry::Type::BadMemory;
                            break;
                        }
                        case STIVALE2_MMAP_ACPI_NVS: {
                            mapEntry.Type = MemmapEntry::Type::AcpiNvs;
                            break;
                        }
                        case STIVALE2_MMAP_RESERVED: {
                            mapEntry.Type = MemmapEntry::Type::Reserved;
                            break;
                        }
                    }

                    if (entry->type != STIVALE2_MMAP_RESERVED)
                        mem._limit =
                            max(mem._limit, entry->base + entry->length);
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

        pTag = tag->next;
    }

    Quark::System::SetupKernel();
}