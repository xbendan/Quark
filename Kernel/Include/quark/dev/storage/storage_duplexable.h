#pragma once

#include <mixins/meta/result.h>
#include <mixins/meta/slice.h>

namespace Quark::System::Io {
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