#include <drivers/acpi/device.h>
#include <mixins/meta/func.h>
#include <quark/api/logging.h>
#include <quark/memory/address_range.h>
#include <quark/memory/address_space.h>

namespace ACPI {
    using Quark::System::Mem::AddressRange;

    static const char* _signature = "RSD PTR ";

    ControllerDevice::ControllerDevice()
        : Device("ACPI Management Device", Class::SystemDevices)
    {
    }

    Res<> ControllerDevice::onLoad()
    {
        u64 address = Mem::copyAsIOAddress( //
            AddressRange(0x0, 0x7bff + 1)
                .find((u8*)_signature, 8, 0x10)
                .orElse(0) |
            AddressRange(0x80000, 0x9ffff + 1)
                .find((u8*)_signature, 8, 0x10)
                .orElse(0) |
            AddressRange(0xe0000, 0xfffff + 1)
                .find((u8*)_signature, 8, 0x10)
                .orElse(0));
        /*
            Detect ACPI Root System Description Pointer (RSDP)
            We cannot initialize ACPI without it.
         */
        if (!address) {
            return Error::DeviceFault("ACPI RSDP not found");
        } else {
            _rsdp = (ACPI::RootSystemDescPointer*)address;
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
        switch (_rsdp->_revision) {
            case 0:
                _rsdt = reinterpret_cast<ACPI::RootSystemDescTable*>(
                    Mem::copyAsIOAddress(_rsdp->_rsdtAddress));
                log(u8"[ACPI] ACPI Version 1.0\n");
                break;
            case 2:
                _xsdp =
                    reinterpret_cast<ACPI::ExtendedSystemDescPointer*>(_rsdp);

                _rsdt = reinterpret_cast<ACPI::RootSystemDescTable*>(
                    Mem::copyAsIOAddress(_rsdp->_rsdtAddress));
                _xsdt = reinterpret_cast<ACPI::ExtendedSystemDescTable*>(
                    Mem::copyAsIOAddress(_xsdp->_xsdtAddress));
                log(u8"[ACPI] ACPI Version 2.0 ~ 6.3\n");
                break;
            default:
                break;
        }

        _madt = findTable<ACPI::MultiApicDescTable>("APIC").unwrap();
        _fadt = findTable<ACPI::FixedAcpiDescTable>("FACP").unwrap();
        _hpet = findTable<ACPI::HighPrecisionEventTable>("HPET").unwrap();
        _mcfg = findTable<ACPI::PCIExpressSpecTable>("MCFG").unwrap();

        return Ok();
    }

    template <class _Tp>
        requires(Std::isDerived<ACPI::TableHeader, _Tp>)
    Res<_Tp*> ControllerDevice::findTable(String<> name, //
                                          unsigned index)
    {
        if (!_rsdp) {
            return Error::DeviceFault("ACPI RSDP not found");
        }

        if (name == "DSDT") {
            return Ok((_Tp*)Mem::copyAsIOAddress(_fadt->_dsdt));
        }

        u64 entries =
            _rsdp->_revision
                ? (_xsdt->_length - sizeof(ACPI::TableHeader) / sizeof(u64))
                : (_rsdt->_length - sizeof(ACPI::TableHeader) / sizeof(u32));
        usize _index = 0;
        for (int i = 0; i < entries; i++) {
            u64 ent =
                _rsdp->_revision ? _xsdt->_pointers[i] : _rsdt->_pointers[i];
            ACPI::TableHeader* table =
                (ACPI::TableHeader*)Mem::copyAsIOAddress(ent);

            if (name.equals(table->_signature) && (_index++ == index)) {
                // Perhaps check the checksum here
                return Ok((_Tp*)table);
            }
        }
        return Error::DeviceFault("Table not found");
    }
} // namespace Quark::System::Hal