#include <drivers/acpi/spec.h>
#include <mixins/std/c++types.h>
#include <quark/dev/device.h>

namespace Quark::Hal {

    class ACPIControllerDevice : public Io::Device
    {
    public:
        ACPIControllerDevice();
        ~ACPIControllerDevice();

        template <typename _Tp>
            requires(Std::isDerived<ACPI::TableHeader, _Tp>)
        Res<_Tp*> findTable(String<> name, unsigned index = 0);

        Res<> onLoad() override;

    private:
        // HashTable<String<>, ACPI::TableHeader*> _tables;

        ACPI::RootSystemDescPointer*     _rsdp;
        ACPI::RootSystemDescTable*       _rsdt;
        ACPI::ExtendedSystemDescPointer* _xsdp;
        ACPI::ExtendedSystemDescTable*   _xsdt;
        ACPI::MultiApicDescTable*        _madt;
        ACPI::HighPrecisionEventTable*   _hpet;
        ACPI::FixedAcpiDescTable*        _fadt;
        ACPI::PCIExpressSpecTable*       _mcfg;
    };
} // namespace Quark::Hal