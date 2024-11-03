#pragma once

#include <mixins/meta/ref.h>
#include <mixins/std/c++types.h>
#include <mixins/str/string.h>
#include <mixins/utils/array_list.h>
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
        Process(u32            processId,
                Qk::StringView name,
                AddressSpace*  addressSpace,
                u64            entryPoint = 0,
                u64            memHeap    = 0,
                u64            memStack   = 0);
        ~Process();

        bool    AddThread(Thread* thread);
        u32     GetNextThreadId() { return m_nextThreadId++; }
        Thread* GetThread(u32 threadId)
        {
            return m_childrenThreadList[threadId];
        }

        u32 const           _processId;
        Qk::StringView      _name;
        AddressSpace* const _addressSpace;
        Thread* const       _mainThread;

        static Process*         GetKernelProcess();
        static IList<Process*>* GetProcessList();

        static Res<RefPtr<Process>> CreateProcess(Qk::StringView name);
        static Res<RefPtr<Process>> CreateIdleProcess();
        static Res<Process*> CreateKernelProcess(AddressSpace* addressSpace);
        static Res<RefPtr<Process>> CreateProcessEx( //
            Qk::StringView name,
            File*          file,
            Folder*        workingDirectory,
            Qk::StringView launchArgs);
        template <typename... TArgs>
        static Res<RefPtr<Process>> CreateProcessEx( //
            Qk::StringView name,
            File*          file,
            Folder*        workingDirectory,
            TArgs&&... launchArgs);
        static void                 DestroyProcess(Process* process);
        static void                 DestroyProcess(u16 id);
        static RefPtr<Process>      GetProcessById(u16 id);

    private:
        LinkedList<Thread*> m_childrenThreadList;

        u64 m_entryPoint;
        u64 m_memHeap;
        u64 m_memStack;
        u64 m_nextThreadId;

        static inline RefPtr<Process>* m_processes;
        static inline Process*         m_kernelProcess;
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
        RefPtr<Process> createProcess(Qk::StringView name);
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
        template <typename... TArgs>
        RefPtr<Process> createProcessEx( //
            Qk::StringView name,
            File*          file,
            Folder*        workingDirectory,
            TArgs&&... launchArgs);
        /**
         * @brief Create a thread with specified parent process
         *
         * @param process
         * @return Thread*
         */
        static Thread*  CreateThread(Process* process);
        static Thread*  CreateThreadEx(Process*       process,
                                       u8             priority,
                                       Qk::StringView name,
                                       Qk::StringView description,
                                       Qk::StringView command,
                                       Qk::StringView arguments);
    };
} // namespace Quark::System::Task