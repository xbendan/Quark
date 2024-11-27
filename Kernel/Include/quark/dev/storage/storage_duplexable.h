#pragma once

#include <mixins/math/align.h>
#include <mixins/meta/result.h>
#include <mixins/meta/slice.h>

#define SECTOR_SIZE 512

namespace Quark::System::Io {

    struct SectorBytes : public Bytes
    {
        SectorBytes(usize size)
            : Bytes(new byte[Math::AlignUp(size, SECTOR_SIZE)],
                    Math::AlignUp(size, SECTOR_SIZE))
        {
        }
        ~SectorBytes() { delete[] buf(); }

        constexpr u32 sectors() const { return len() / SECTOR_SIZE; }
    };

    enum Direction
    {
        ACCESS_READ  = 0,
        ACCESS_WRITE = 1
    };

    class StorageDuplexable
    {
    public:
        virtual Res<usize> Read(u64, Bytes)  = 0;
        virtual Res<usize> Write(u64, Bytes) = 0;

    protected:
        StorageDuplexable(usize bufferSize)
            : m_bufferSize(bufferSize)
        {
        }

    private:
        usize m_bufferSize;
    };
}