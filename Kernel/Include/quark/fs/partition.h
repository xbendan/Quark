#pragma once

#include <mixins/math/range.h>
#include <mixins/std/c++types.h>
#include <mixins/utils/array.h>
#include <mixins/utils/uuid.h>

namespace Quark::System::Io::FileSystem {
    using Qk::Array;

    class Partition
    {
    public:
        Partition() = default;
        Partition(u64 lbaBegin, u64 lbaEnd)
            : m_lbaBegin(lbaBegin)
            , m_lbaEnd(lbaEnd)
        {
        }

        ~Partition() = default;

        u64 GetLBABegin() const { return m_lbaBegin; }
        u64 GetLBAEnd() const { return m_lbaEnd; }

    private:
        u64 m_lbaBegin;
        u64 m_lbaEnd;
    };

    class PartitionTable
    {
    public:
        Opt<Partition>          GetPartitionByIndex(u64 index) const;
        Opt<Partition>          GetPartitionByUuid(UUID const& uuid) const;
        usize                   Count() const;
        Array<Partition> const& EnumerateValidPartitions() const;

    protected:
        Array<Partition> m_partitions;
    };
}