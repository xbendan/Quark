#pragma once

#include <mixins/meta/ref.h>
#include <mixins/std/c++types.h>
#include <mixins/std/string.h>
#include <mixins/utils/linked_list.h>
#include <quark/fs/file.h>
#include <quark/memory/address_space.h>
#include <quark/sched/thread.h>

namespace Quark::System::Task {
    using namespace Quark::System::Memory;
    using namespace Quark::System::Io::FileSystem;

    class Process
    {
    public:
        Process(u32           processId,
                string        name,
                AddressSpace* addressSpace,
                u64           entryPoint = 0,
                u64           memHeap    = 0,
                u64           memStack   = 0);
        ~Process();

        bool    addThread(Thread* thread);
        Thread* getThread(u32 threadId)
        {
            return m_childrenThreadList[threadId];
        }

        u32 const           _processId;
        string              _name;
        AddressSpace* const _addressSpace;
        Thread* const       _mainThread;

    private:
        LinkedList<Thread*> m_childrenThreadList;

        u64 m_entryPoint;
        u64 m_memHeap;
        u64 m_memStack;
        u64 m_nextThreadId;
    };

    class ProcessFactory
    {
    public:
        /**
         * @brief Create a Process with specified name
         * The process will not be started until it has a valid entry point
         * with a existed file.
         *
         * @param name
         * @return Process* pointer to the created process
         */
        RefPtr<Process> createProcess(string name);
        /**
         * @brief Create an idle process
         * The idle process will be started immediately after it is created,
         * but consume no resources.
         *
         * @return Process*
         */
        RefPtr<Process> createIdleProcess();
        /**
         * @brief Create a process with full arguments
         *
         * @param name
         * @return Process*
         */
        template <typename... Args>
        RefPtr<Process> createProcessEx( //
            string  name,
            File*   file,
            Folder* workingDirectory,
            Args&&... launchArgs);
        /**
         * @brief Create a thread with specified parent process
         *
         * @param process
         * @return Thread*
         */
        Thread*         createThread(Process* process);
        Thread*         createThreadEx(Process*      process,
                                       u8            priority,
                                       const string& name,
                                       const string& description,
                                       const string& command,
                                       const string& arguments);
    };
} // namespace Quark::System::Task