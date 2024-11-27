#include <drivers/acpi/device.h>
#include <drivers/acpi/timer.h>
#include <mixins/meta/func.h>
#include <mixins/utils/strings.h>
#include <quark/memory/address_range.h>
#include <quark/memory/address_space.h>
#include <quark/os/diagnostic/logging.h>

namespace ACPI {
    using namespace Quark::System::Diagnostic;
    using namespace Quark::System;
    using Quark::System::Memory::AddressRange;

    static const char* _signature = "RSD PTR ";

    ControllerDevice::ControllerDevice()
        : Device(Name, Type::SystemDevices)
    {
    }

    Res<> ControllerDevice::OnInitialize()
    {
        u64 address = Memory::CopyAsIOAddress( //
            AddressRange(0x0, 0x7bff + 1)
                .Find((u8*)_signature, 8, 0x10)
                .OrElse(0) |
            AddressRange(0x80000, 0x9ffff + 1)
                .Find((u8*)_signature, 8, 0x10)
                .OrElse(0) |
            AddressRange(0xe0000, 0xfffff + 1)
                .Find((u8*)_signature, 8, 0x10)
                .OrElse(0));
        /*
            Detect ACPI Root System Description Pointer (RSDP)
            We cannot initialize ACPI without it.
         */
        if (!address) {
            return Error::DeviceFault("ACPI RSDP not found");
        } else {
            TblRsdp = (ACPI::RootSystemDescPointer*)address;
        }

        /*
            Handle ACPI Revision
            When Revision
             ->  2           = ACPI Version 2.0 ~ 6.3
                               ACPI 2.0+ contains an extended structure so RSDP
                               needs to be cast into XSDP
             ->  0           = ACPI Version 1.0
            If non of them above matches the revision, call kernel panic
            because there shouldn't be any exceptions.
         */
        switch (TblRsdp->_revision) {
            case 0:
                TblRsdt = reinterpret_cast<ACPI::RootSystemDescTable*>(
                    Memory::CopyAsIOAddress(TblRsdp->_rsdtAddress));
                info("[ACPI] ACPI Version 1.0");
                break;
            case 2:
                TblXsdp =
                    reinterpret_cast<ACPI::ExtendedSystemDescPointer*>(TblRsdp);

                TblRsdt = reinterpret_cast<ACPI::RootSystemDescTable*>(
                    Memory::CopyAsIOAddress(TblRsdp->_rsdtAddress));
                TblXsdt = reinterpret_cast<ACPI::ExtendedSystemDescTable*>(
                    Memory::CopyAsIOAddress(TblXsdp->_xsdtAddress));
                info("[ACPI] ACPI Version 2.0 ~ 6.3\n");
                break;
            default:
                break;
        }

        TblMadt = FindTable<ACPI::MultiApicDescTable>("APIC");
        TblFadt = FindTable<ACPI::FixedAcpiDescTable>("FACP");
        TblHpet = FindTable<ACPI::HighPrecisionEventTable>("HPET");
        TblMcfg = FindTable<ACPI::PCIExpressSpecTable>("MCFG");

        Device::Load(new ACPITimerDevice(this));

        return Ok();
    }

    ACPI::TableHeader* ControllerDevice::FindTableBase(Qk::StringView name, //
                                                       unsigned       index)
    {
        if (!TblRsdp) {
            return nullptr;
        }

        if (name == "DSDT") {
            return (ACPI::TableHeader*)Memory::CopyAsIOAddress(TblFadt->_dsdt);
        }

        u64 entries =
            TblRsdp->_revision
                ? (TblXsdt->_length - sizeof(ACPI::TableHeader) / sizeof(u64))
                : (TblRsdt->_length - sizeof(ACPI::TableHeader) / sizeof(u32));
        usize _index = 0;
        for (int i = 0; i < entries; i++) {
            u64                ent = TblRsdp->_revision ? TblXsdt->_pointers[i]
                                                        : TblRsdt->_pointers[i];
            ACPI::TableHeader* table =
                (ACPI::TableHeader*)Memory::CopyAsIOAddress(ent);

            if (Qk::Strings::Equals(table->_signature, name) &&
                (_index++ == index)) {
                info$("[ACPI] ACPI Table found: {}, at {#X}", name, ent);

                // Perhaps check the checksum here
                return table;
            }
        }

        warn$("[ACPI] ACPI Table not found: {}", name);
        return nullptr;
    }
} // namespace Quark::System::Hal