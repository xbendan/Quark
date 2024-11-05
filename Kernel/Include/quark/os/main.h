#include <mixins/meta/opt.h>
#include <mixins/meta/ref.h>
#include <mixins/meta/result.h>
#include <mixins/utils/array.h>
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

    [[noreturn]]
    void  SetupKernel();
    Res<> SetupArch();

    Res<>                        InitPhysMemory();
    Res<AddressSpace*>           InitVirtMemory();
    Res<Qk::Array<Io::Device*>*> EnumerateInitialDevices();
    Res<IList<Io::Device*>*>     SetupDevices();
    Res<>                        InitKernelMemory();
    Res<>                        InitTasks();
    Res<>                        InitFileSystems();

} // namespace Quark::System