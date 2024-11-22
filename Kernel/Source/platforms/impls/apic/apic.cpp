#include <drivers/acpi/device.h>
#include <drivers/apic/device.h>
#include <drivers/apic/spec.h>
#include <platforms/x86_64/cpu.h>
#include <platforms/x86_64/sched.h>
#include <quark/hal/interrupts.h>
#include <quark/hal/ports.h>
#include <quark/os/diagnostic/logging.h>

namespace Quark::System {
    using namespace Quark::System::Platform::X64;

    extern Inert<CPULocalDevice> kCPULocal;
}

namespace APIC {
    using namespace Platform::X64;
    using namespace Quark::System::Diagnostic;
    using namespace Quark::System::Hal;

    static GenericControllerDevice* _Device;

    void EndOfInterrupt()
    {
        _Device->WriteRegLoc(LOCAL_APIC_EOI, 0);
    }

    void EnableInterrupt(u8 irq, u64 data)
    {
        _Device->WriteReg64(IO_APIC_RED_TABLE_ENT(irq), data);
    }

    void EnableInterrupt(u8 irq)
    {
        irq -= 0x20;
        int nirq = -1;
        for (auto* iso : _Device->GetInterruptOverrides()) {
            if (iso->_irqSource == irq) {
                nirq = iso->_gSi;
                break;
            }
        }
        if (nirq == -1) {
            warn$("[APIC] No interrupt override found for IRQ {}", irq);
            nirq = irq;
        }
        EnableInterrupt(nirq, irq);
    }

    bool __SpuriousInterrupt(int, Registers*)
    {
        log("Spurious interrupt");
        return true;
    }

    GenericControllerDevice::GenericControllerDevice()
        : Device(GenericControllerDevice::Name, Type::SystemDevices)
    {
        _Device = this;
    }

    Res<> GenericControllerDevice::OnInitialize()
    {
        asm volatile("cli");
        ACPI::MADT* madt = nullptr;

        auto devRes = Device::FindByName<ACPI::ControllerDevice>(
            "ACPI Management Device");
        devRes.IfPresent([&madt](ACPI::ControllerDevice* acpi) {
            Opt<ACPI::MADT*> opt = acpi->FindTable<ACPI::MADT>("APIC");
            if (opt.IsPresent())
                madt = opt.Take("MADT table not found.");
        });
        assert(madt != nullptr, "MADT table not found.");

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
                        CPULocalDevice* device;
                        if (apicLocal->_processorId == 0) {
                            device = reinterpret_cast<CPULocalDevice*>(
                                kCPULocal._data);
                            info$("Set CPU0 to {:#X}", device);
                        } else {
                            device = new CPULocalDevice(apicLocal->_processorId,
                                                        nullptr);
                            Device::Load(device);
                        }

                        m_units.PushBack(
                            new Local(apicLocal->_apicId, this, device));
                        info$(
                            "[APIC] Local APIC, Processor ID: {}, APIC ID: {}, "
                            "Flags: {#b}",
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
                        info$(
                            "[APIC] I/O APIC, ID: {}, Address: {#X}, gSiB: {}",
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
                    m_overrides.PushBack(iso);
                    info$("[APIC] Interrupt Service Override, Bus: {}, "
                          "IRQ: {}, gSi: {}",
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
                    info$("[APIC] NMI Source: {}, Flags: {}, LINT: {}",
                          nmi->_processorId,
                          nmi->_flags,
                          nmi->_lInt);
                    break;
                }
                case 0x04: /* Local APIC Non-maskable Interrupt */ {
                    // ACPI::MultiApicDescTable::Nmix2Apic* nmi =
                    //     static_cast<ACPI::MultiApicDescTable::Nmix2Apic*>(
                    //         entry);
                    // log(u8"[APIC] NMI Source: %d, Local APIC LINT: %d\n",
                    //     nmi->_uid,
                    //     nmi->_lInt);
                    break;
                }
                case 0x05: /* Local APIC Address Override */ {
                    ACPI::MultiApicDescTable::LocalApic* apicLocal =
                        static_cast<ACPI::MultiApicDescTable::LocalApic*>(
                            entry);
                    info$("[APIC] Local APIC Address Override, Processor ID: "
                          "{}, APIC ID: {}, Flags: {}",
                          apicLocal->_processorId,
                          apicLocal->_apicId,
                          apicLocal->_flags);
                    break;
                }
                case 0x09: /* Processor Local x2APIC */ {
                    ACPI::MultiApicDescTable::Localx2Apic* x2apicLocal =
                        static_cast<ACPI::MultiApicDescTable::Localx2Apic*>(
                            entry);
                    info$("[APIC] Local x2APIC, x2APIC ID: {}, Flags: {}, UID: "
                          "{}",
                          x2apicLocal->_x2apicId,
                          x2apicLocal->_flags,
                          x2apicLocal->_uid);
                    break;
                }
                case 0xa: /* Local x2APIC Non-maskable Interrupt */ {
                    ACPI::MultiApicDescTable::Nmix2Apic* nmi =
                        static_cast<ACPI::MultiApicDescTable::Nmix2Apic*>(
                            entry);
                    info$("[APIC] NMI Source: {}, UID: {}, Local APIC LINT: {}",
                          nmi->_flags,
                          nmi->_uid,
                          nmi->_lInt);
                    break;
                }
            }
            offset += entry->_length;
        }

        if (!m_ioBasePhys) {
            warn("[APIC] No I/O APIC found");
            return Error::DeviceFault("No I/O APIC found");
        }

        // pOut<u8>(0x20, 0x11);
        // pOut<u8>(0xA0, 0x11);
        // pOut<u8>(0x21, 0x20);
        // pOut<u8>(0xA1, 0x28);
        // pOut<u8>(0x21, 0x04);
        // pOut<u8>(0xA1, 0x02);
        // pOut<u8>(0x21, 0x01);
        // pOut<u8>(0xA1, 0x01);
        pOut<u8>(0x21, 0xFF);
        pOut<u8>(0xA1, 0xFF);

        m_ioBaseVirt = Memory::CopyAsIOAddress(m_ioBasePhys);
        m_ioRegSel   = (u32*)(m_ioBaseVirt + IO_APIC_REGSEL);
        m_ioWindow   = (u32*)(m_ioBaseVirt + IO_APIC_WIN);
        m_interrupts = ReadReg32(IO_APIC_REGISTER_VER) >> 16;

        Hal::EnableInterrupt(0xFF, __SpuriousInterrupt);

        if (Platform::X64::rdmsr(MSR_APIC_BASE) != 0) {
        }

        GetApicLocal(0)->Enable();

        // Initialize I/O APIC
        // for (auto* iso : m_overrides) {
        //     Redirect(iso->_gSi, iso->_irqSource + 0x20, 0);
        // }

        return Ok();
    }

    void GenericControllerDevice::WriteReg32(u32 reg, u32 data)
    {
        *m_ioRegSel = reg;
        *m_ioWindow = data;
    }

    u32 GenericControllerDevice::ReadReg32(u32 reg)
    {
        *m_ioRegSel = reg;
        return *m_ioWindow;
    }

    void GenericControllerDevice::WriteReg64(u32 reg, u64 data)
    {
        u32 low = data & 0xFFFFFFFF, high = data >> 32;

        WriteReg32(reg, low);
        WriteReg32(reg + 1, high);
    }

    u64 GenericControllerDevice::ReadReg64(u32 reg)
    {
        u32 low = ReadReg32(reg), high = ReadReg32(reg + 1);

        return (u64)high << 32 | low;
    }

    void GenericControllerDevice::WriteRedTbl(u32 index, u64 data)
    {
        WriteReg64(IO_APIC_RED_TABLE_ENT(index), data);
    }

    u64 GenericControllerDevice::ReadRedTbl(u32 index)
    {
        return ReadReg64(IO_APIC_RED_TABLE_ENT(index));
    }

    void GenericControllerDevice::WriteBase(u64 data)
    {
        Platform::X64::wrmsr(MSR_APIC_BASE, data);
    }

    u64 GenericControllerDevice::ReadBase()
    {
        return Platform::X64::rdmsr(MSR_APIC_BASE);
    }

    void GenericControllerDevice::WriteRegLoc(u32 reg, u32 data)
    {
        *((volatile u32*)(m_ioBaseVirt + reg)) = data;
    }

    u32 GenericControllerDevice::ReadRegLoc(u32 reg)
    {
        return *((volatile u32*)(m_ioBaseVirt + reg));
    }

    void GenericControllerDevice::Redirect(u8 irq, u8 vector, u32 delivery)
    {
        WriteReg64(IO_APIC_RED_TABLE_ENT(irq), delivery | vector);
    }

} // namespace Quark::System::Hal