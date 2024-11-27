#include <drivers/pci/enumeration.h>

#include <drivers/storage/ahci/ahci_controller_device.h>
#include <drivers/storage/ide/ide_controller_device.h>
#include <quark/os/diagnostic/logging.h>

namespace PCI {
    using namespace Quark::System::Diagnostic;

    PCI::Device* CreateMassStorageDevice(PCI::DeviceIdentifier& info)
    {
        // clang-format off
        switch (info.GetSubclass()) {
            case 0x01: return new IDE::StorageControllerDevice(info);
            // case 0x06: return new AHCI::StorageControllerDevice(info);
        }
        // clang-format on
        return nullptr;
    }

    PCI::Device* PCIEnumerationDevice::CreateEnumeratedDevice(
        PCI::DeviceIdentifier& info)
    {
        PCI::Device* device = nullptr;
        // clang-format off
        switch ((ClassCode)info.GetClass()) {
            case ClassCode::MASS_STORAGE: { device = CreateMassStorageDevice(info); break; }
            case ClassCode::NETWORK: { break; }
            case ClassCode::DISPLAY: { break; }
            case ClassCode::MULTIMEDIA: { break; }
            case ClassCode::MEMORY: { break; }
            case ClassCode::BRIDGE: { break; }
            case ClassCode::COMMUNICATION: { break; }
            case ClassCode::PERIPHERAL: { break; }
            case ClassCode::INPUT_DEVICE: { break; }
            case ClassCode::DOCKING_STATION: { break; }
            case ClassCode::PROCESSOR: { break; }
            case ClassCode::SERIAL_BUS: { break; }
            case ClassCode::WIRELESS: { break; }
            case ClassCode::INTELLIGENT: { break; }
            case ClassCode::SATELLITE: { break; }
            case ClassCode::ENCRYPTION: { break; }
            case ClassCode::SIGNAL_PROCESSING: { break; }
            case ClassCode::ACCELERATOR: { break; }
            case ClassCode::NON_ESSENTIAL_INSTRUMENTATION: { break; }
            case ClassCode::COPROCESSOR: { break; }
            case ClassCode::UNCLASSIFIED: { break; }
            default: {
                warn$("[PCI] Device instance has an unrecognizable class: {:#u}", info.GetClass());
                break;
            }
        }
        // clang-format on

        if (device == nullptr) {
            // warn$("[PCI] Unhandled device instance class: {:#X} at "
            //       "{:#u}:{:#u}:{:#u}",
            //       info.GetClass(),
            //       info._bus,
            //       info._slot,
            //       info._func);
            device = new PCI::Device(info);
        }

        return device;
    }
}