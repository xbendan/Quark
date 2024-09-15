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
    void  setupKernel(LaunchConfiguration* bootInfo);
    Res<> setupArch(LaunchConfiguration* bootInfo);

    Res<>                            initPhysMemory();
    Res<AddressSpace*>               initVirtMemory();
    Res<Process*>                    createKernelProcess(AddressSpace*);
    Res<IReadOnlyList<Io::Device*>*> enumerateInitialDevices();
    Res<IReadOnlyList<Io::Device*>*> setupDevices();
    Res<>                            initTasks();
    Res<>                            initFileSystems();

} // namespace Quark::System