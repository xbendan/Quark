#include <mixins/meta/opt.h>
#include <mixins/meta/ref.h>
#include <mixins/meta/result.h>
#include <mixins/utils/uuid.h>

#include <quark/dev/device.h>
#include <quark/init/boot_info.h>
#include <quark/init/boot_modules.h>
#include <quark/memory/address_space.h>
#include <quark/memory/page.h>
#include <quark/sched/process.h>
#include <quark/sched/sched.h>
#include <quark/sched/thread.h>

namespace Quark::System {
    using Quark::System::Memory::AddressSpace;
    using Quark::System::Task::Process;

    LaunchConfiguration& getLaunchConfiguration();

    [[noreturn]]
    void  SetupKernel(LaunchConfiguration* bootInfo);
    Res<> SetupArch(LaunchConfiguration* bootInfo);

    Res<>                            InitPhysMemory();
    Res<AddressSpace*>               InitVirtMemory();
    Res<Process*>                    CreateKernelProcess(AddressSpace*);
    Res<IReadOnlyList<Io::Device*>*> enumerateInitialDevices();
    Res<IReadOnlyList<Io::Device*>*> SetupDevices();
    Res<>                            InitTasks();
    Res<>                            InitFileSystems();

} // namespace Quark::System