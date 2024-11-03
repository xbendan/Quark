#include <drivers/acpi/device.h>
#include <drivers/apic/device.h>
#include <drivers/apic/spec.h>
#include <platforms/x86_64/cpu.h>
#include <platforms/x86_64/sched.h>
#include <quark/os/diagnostic/logging.h>

namespace APIC {
    using namespace Platform::X64;
    using namespace Quark::System::Diagnostic;

    GenericControllerDevice::GenericControllerDevice()
        : Device("Advanced Programmable Interrupt Controller",
                 Type::SystemDevices)
        , m_units(new LinkedList<Local*>())
        , m_overrides(new LinkedList<ACPI::MADT::InterruptServiceOverride*>())
    {
    }

    Res<> GenericControllerDevice::OnInitialize()
    {
        ACPI::MADT* madt = nullptr;

        auto devRes = Device::FindByName<ACPI::ControllerDevice>(
            "ACPI Management Device");
        devRes.IfPresent([&madt](ACPI::ControllerDevice* acpi) {
            Opt<ACPI::MADT*> opt = acpi->FindTable<ACPI::MADT>("APIC");
            if (opt.IsPresent())
                madt = opt.Take("MADT table not found.");
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
                        CPULocalDevice* device = new CPULocalDevice(
                            apicLocal->_processorId, nullptr);
                        Device::Load(device);

                        m_units->pushBack(
                            new Local(apicLocal->_apicId, this, device));
                        info$(
                            "[APIC] Local APIC, Processor ID: {}, APIC ID: {}, "
                            "Flags: {}",
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
                        info$("[APIC] I/O APIC, ID: {}, Address: {#x}, Global "
                              "System Interrupt Base: {}",
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
                    info$("[APIC] Interrupt Service Override, Bus={}, "
                          "IRQ={}, gSI={}",
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
                    info$("[APIC] NMI Source: {}, Global System Interrupt: {}",
                          nmi->_processorId,
                          nmi->_type);
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
            // TODO: log
            return Error::DeviceFault("No I/O APIC found");
        }

        m_ioBaseVirt = Memory::CopyAsIOAddress(m_ioBasePhys);
        m_ioRegSel   = (u32*)(m_ioBaseVirt + IO_APIC_REGSEL);
        m_ioWindow   = (u32*)(m_ioBaseVirt + IO_APIC_WIN);
        m_interrupts = ioRegRead32(IO_APIC_REGISTER_VER) >> 16;

        if (Platform::X64::rdmsr(MSR_APIC_BASE) != 0) {
        }

        return Ok();
    }

    void GenericControllerDevice::ioRegWrite32(u32 reg, u32 data)
    {
        *m_ioRegSel = reg;
        *m_ioWindow = data;
    }

    u32 GenericControllerDevice::ioRegRead32(u32 reg)
    {
        *m_ioRegSel = reg;
        return *m_ioWindow;
    }

    void GenericControllerDevice::ioRegWrite64(u32 reg, u64 data)
    {
        u32 low = data & 0xFFFFFFFF, high = data >> 32;

        ioRegWrite32(reg, low);
        ioRegWrite32(reg + 1, high);
    }

    u64 GenericControllerDevice::ioRegRead64(u32 reg)
    {
        u32 low = ioRegRead32(reg), high = ioRegRead32(reg + 1);

        return (u64)high << 32 | low;
    }

    void GenericControllerDevice::ioRedTblWrite(u32 index, u64 data)
    {
        ioRegWrite64(IO_APIC_RED_TABLE_ENT(index), data);
    }

    u64 GenericControllerDevice::ioRedTblRead(u32 index)
    {
        return ioRegRead64(IO_APIC_RED_TABLE_ENT(index));
    }

    void GenericControllerDevice::localBaseWrite(u64 data)
    {
        Platform::X64::wrmsr(MSR_APIC_BASE, data);
    }

    u64 GenericControllerDevice::localBaseRead()
    {
        return Platform::X64::rdmsr(MSR_APIC_BASE);
    }

    void GenericControllerDevice::localRegWrite(u32 reg, u32 data)
    {
        *((volatile u32*)(m_ioBaseVirt + reg)) = data;
    }

    u32 GenericControllerDevice::localRegRead(u32 reg)
    {
        return *((volatile u32*)(m_ioBaseVirt + reg));
    }

} // namespace Quark::System::Hal