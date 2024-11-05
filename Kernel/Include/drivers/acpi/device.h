#include <drivers/acpi/spec.h>
#include <mixins/std/c++types.h>
#include <quark/dev/device.h>

namespace ACPI {
    using Quark::System::Io::Device;

    class ControllerDevice : public Device
    {
    public:
        ControllerDevice();
        ~ControllerDevice() = default;

        ACPI::TableHeader* FindTableBase(Qk::StringView name,
                                         unsigned       index = 0);

        template <typename T>
            requires(Std::isDerived<ACPI::TableHeader, T>)
        inline T* FindTable(Qk::StringView name, unsigned index = 0)
        {
            return static_cast<T*>(FindTableBase(name, index));
        }

        Res<> OnInitialize() override;

        static inline const char* Name = "ACPI Management Device";
        // HashTable<string, ACPI::TableHeader*> _tables;

        ACPI::RootSystemDescPointer*     TblRsdp;
        ACPI::RootSystemDescTable*       TblRsdt;
        ACPI::ExtendedSystemDescPointer* TblXsdp;
        ACPI::ExtendedSystemDescTable*   TblXsdt;
        ACPI::MultiApicDescTable*        TblMadt;
        ACPI::HighPrecisionEventTable*   TblHpet;
        ACPI::FixedAcpiDescTable*        TblFadt;
        ACPI::PCIExpressSpecTable*       TblMcfg;
    };
} // namespace Quark::System::Hal