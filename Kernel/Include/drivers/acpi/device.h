#include <drivers/acpi/spec.h>
#include <mixins/std/c++types.h>
#include <quark/dev/device.h>

namespace ACPI {
    using namespace Quark::System;

    class ControllerDevice : public Io::Device
    {
    public:
        ControllerDevice();
        ~ControllerDevice() = default;

        Optional<ACPI::TableHeader*> FindTableBase(string   name,
                                                   unsigned index = 0);

        template <typename T>
            requires(Std::isDerived<ACPI::TableHeader, T>)
        inline Optional<T*> FindTable(string name, unsigned index = 0)
        {
            auto opt = FindTableBase(name, index);
            if (opt.IsPresent()) {
                return static_cast<T*>(opt.Take());
            }
            return Empty();
        }

        Res<> OnInitialize() override;

    private:
        // HashTable<string, ACPI::TableHeader*> _tables;

        ACPI::RootSystemDescPointer*     _rsdp;
        ACPI::RootSystemDescTable*       _rsdt;
        ACPI::ExtendedSystemDescPointer* _xsdp;
        ACPI::ExtendedSystemDescTable*   _xsdt;
        ACPI::MultiApicDescTable*        _madt;
        ACPI::HighPrecisionEventTable*   _hpet;
        ACPI::FixedAcpiDescTable*        _fadt;
        ACPI::PCIExpressSpecTable*       _mcfg;
    };
} // namespace Quark::System::Hal