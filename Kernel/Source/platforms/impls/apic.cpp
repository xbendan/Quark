#include <drivers/acpi/device.h>
#include <drivers/apic/device.h>
#include <drivers/apic/spec.h>
#include <platforms/x86_64/cpu.h>
#include <quark/api/device.h>
#include <quark/api/logging.h>

namespace Quark::Hal {
    using namespace API;

    ApicDevice::ApicDevice()
        : Device("Advanced Programmable Interrupt Controller",
                 Class::SystemDevices)
        , m_units(new LinkedList<Local*>())
        , m_overrides(new LinkedList<ACPI::MADT::InterruptServiceOverride*>())
    {
    }

    Res<> ApicDevice::onLoad()
    {
        ACPI::MADT* madt;

        ::getRegisteredDevice<ACPIControllerDevice>("ACPI Management Device")
            .ifPresent([&madt](ACPIControllerDevice* acpi) {
                Res<ACPI::MADT*> opt = acpi->findTable<ACPI::MADT>("APIC");
                if (opt.has())
                    madt = opt.unwrap("MADT table not found.");
            });
        if (madt == nullptr)
            return Error::DeviceFault("MADT table not found.");

        u64 offset = (u64) & (madt->_entries[0]);
        u64 end    = (u64)madt + madt->_length;
        while (offset < end) {
            ACPI::MultiApicDescTable::Entry* entry =
                reinterpret_cast<ACPI::MultiApicDescTable::Entry*>(offset);

            switch (entry->_type) {
                case 0x00: /* Processor Local APIC */ {
                    ACPI::MultiApicDescTable::LocalApic* apicLocal =
                        static_cast<ACPI::MultiApicDescTable::LocalApic*>(
                            entry);
                    if (apicLocal->_flags & 0x3) {
                        ProcessorDevice* processor = new ProcessorDevice(
                            apicLocal->_processorId, nullptr);
                        ::registerDevice(processor);

                        m_units->pushBack(
                            new Local(apicLocal->_apicId, this, processor));
                        log("[APIC] Local APIC, Processor ID: %d, APIC ID: %d, "
                            "Flags: %d\n",
                            apicLocal->_processorId,
                            apicLocal->_apicId,
                            apicLocal->_flags);
                    }
                    break;
                }
                case 0x01: /* IO APIC */ {
                    ACPI::MultiApicDescTable::IoApic* apicIo =
                        static_cast<ACPI::MultiApicDescTable::IoApic*>(entry);
                    if (!apicIo->_gSiB) {
                        m_ioBasePhys = apicIo->_address;
                        log("[APIC] I/O APIC, ID: %d, Address: %x, Global "
                            "System Interrupt Base: %d\n",
                            apicIo->_apicId,
                            apicIo->_address,
                            apicIo->_gSiB);
                    }
                    break;
                }
                case 0x02: /* Interrupt Source Override */ {
                    ACPI::MultiApicDescTable::InterruptServiceOverride* iso =
                        static_cast<ACPI::MultiApicDescTable::
                                        InterruptServiceOverride*>(entry);
                    m_overrides->pushBack(iso);
                    log("[APIC] Interrupt Service Override, Bus Source: %d, "
                        "IRQ Source: %d, Global System Interrupt: %d\n",
                        iso->_busSource,
                        iso->_irqSource,
                        iso->_gSi);

                    break;
                }
                case 0x03: /* Non-maskable Interrupt */ {
                    ACPI::MultiApicDescTable::NonMaskableInterrupt* nmi =
                        static_cast<
                            ACPI::MultiApicDescTable::NonMaskableInterrupt*>(
                            entry);
                    log("[APIC] NMI Source: %d, Global System Interrupt: %d\n",
                        nmi->_processorId,
                        nmi->_type);
                    break;
                }
                case 0x04: /* Local APIC Non-maskable Interrupt */ {
                    // ACPI::MultiApicDescTable::Nmix2Apic* nmi =
                    //     static_cast<ACPI::MultiApicDescTable::Nmix2Apic*>(
                    //         entry);
                    // log("[APIC] NMI Source: %d, Local APIC LINT: %d\n",
                    //     nmi->_uid,
                    //     nmi->_lInt);
                    break;
                }
                case 0x05: /* Local APIC Address Override */ {
                    ACPI::MultiApicDescTable::LocalApic* apicLocal =
                        static_cast<ACPI::MultiApicDescTable::LocalApic*>(
                            entry);
                    log("[APIC] Local APIC Address Override, Processor ID: %d, "
                        "APIC ID: %d, Flags: %d\n",
                        apicLocal->_processorId,
                        apicLocal->_apicId,
                        apicLocal->_flags);
                    break;
                }
                case 0x09: /* Processor Local x2APIC */ {
                    ACPI::MultiApicDescTable::Localx2Apic* x2apicLocal =
                        static_cast<ACPI::MultiApicDescTable::Localx2Apic*>(
                            entry);
                    log("[APIC] Local x2APIC, x2APIC ID: %d, Flags: %d, UID: "
                        "%d\n",
                        x2apicLocal->_x2apicId,
                        x2apicLocal->_flags,
                        x2apicLocal->_uid);
                    break;
                }
                case 0xa: /* Local x2APIC Non-maskable Interrupt */ {
                    ACPI::MultiApicDescTable::Nmix2Apic* nmi =
                        static_cast<ACPI::MultiApicDescTable::Nmix2Apic*>(
                            entry);
                    log("[APIC] NMI Source: %d, UID: %d, Local APIC LINT: %d\n",
                        nmi->_flags,
                        nmi->_uid,
                        nmi->_lInt);
                    break;
                }
            }
            offset += entry->_length;
        }

        if (!m_ioBasePhys) {
            // TODO: log
            return Error::DeviceFault("No I/O APIC found");
        }

        m_ioBaseVirt = Mem::copyAsIOAddress(m_ioBasePhys);
        m_ioRegSel   = (u32*)(m_ioBaseVirt + IO_APIC_REGSEL);
        m_ioWindow   = (u32*)(m_ioBaseVirt + IO_APIC_WIN);
        m_interrupts = ioRegRead32(IO_APIC_REGISTER_VER) >> 16;

        if (Platform::X64::rdmsr(MSR_APIC_BASE) != 0) {
        }

        return Ok();
    }

    void ApicDevice::ioRegWrite32(u32 reg, u32 data)
    {
        *m_ioRegSel = reg;
        *m_ioWindow = data;
    }

    u32 ApicDevice::ioRegRead32(u32 reg)
    {
        *m_ioRegSel = reg;
        return *m_ioWindow;
    }

    void ApicDevice::ioRegWrite64(u32 reg, u64 data)
    {
        u32 low = data & 0xFFFFFFFF, high = data >> 32;

        ioRegWrite32(reg, low);
        ioRegWrite32(reg + 1, high);
    }

    u64 ApicDevice::ioRegRead64(u32 reg)
    {
        u32 low = ioRegRead32(reg), high = ioRegRead32(reg + 1);

        return (u64)high << 32 | low;
    }

    void ApicDevice::ioRedTblWrite(u32 index, u64 data)
    {
        ioRegWrite64(IO_APIC_RED_TABLE_ENT(index), data);
    }

    u64 ApicDevice::ioRedTblRead(u32 index)
    {
        return ioRegRead64(IO_APIC_RED_TABLE_ENT(index));
    }

    void ApicDevice::localBaseWrite(u64 data)
    {
        Platform::X64::wrmsr(MSR_APIC_BASE, data);
    }

    u64 ApicDevice::localBaseRead()
    {
        return Platform::X64::rdmsr(MSR_APIC_BASE);
    }

    void ApicDevice::localRegWrite(u32 reg, u32 data)
    {
        *((volatile u32*)(m_ioBaseVirt + reg)) = data;
    }

    u32 ApicDevice::localRegRead(u32 reg)
    {
        return *((volatile u32*)(m_ioBaseVirt + reg));
    }

} // namespace Quark::Hal