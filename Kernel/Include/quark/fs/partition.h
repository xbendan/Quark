#pragma once

#include <mixins/math/range.h>
#include <mixins/meta/ref.h>
#include <mixins/meta/result.h>
#include <mixins/std/c++types.h>
#include <mixins/utils/array.h>
#include <mixins/utils/uuid.h>

namespace Quark::System::Io::FileSystem {
    using Qk::Array;
    using Qk::NonnullRefPtr;
    using Qk::Result;

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
        Optional<Partition> GetPartitionByIndex(u64 index) const;
        Optional<Partition> GetPartitionByUuid(UUID const& uuid) const;
        usize               Count() const;

        Partition&       operator[](usize index) { return m_partitions[index]; }
        Partition const& operator[](usize index) const
        {
            return m_partitions[index];
        }

        Partition* begin() { return m_partitions.begin(); }
        Partition* end() { return m_partitions.end(); }

    protected:
        Array<Partition> m_partitions;
    };
}