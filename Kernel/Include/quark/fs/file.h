#pragma once

#include <mixins/meta/result.h>
#include <mixins/std/c++types.h>
#include <mixins/str/string.h>
#include <mixins/utils/array.h>
#include <mixins/utils/flags.h>
#include <mixins/utils/uuid.h>

namespace Quark::System::Io::FileSystem {
    using Qk::StringView;

    enum class FileOpenMode
    {
        Append,
        Create,
        CreateIfAbsent,
        Open,
        OpenOrCreate,
    };

    enum class FileAttribute : u64
    {
        ReadOnly = 1,
        Hidden,
        Archive,
        Compressed,
        Encrypted,
        Temporary
    };
    MakeFlags$(FileAttribute);

    enum class FileSource
    {
        Local,
        Network,
        Cloud,
        Virtual,
        Device
    };

    class FileNode
    {
    public:
        FileNode(FileSource source, Qk::StringView name, Qk::StringView parent)
            : m_source(source)
            , m_name(name)
            , m_parent(parent)
        {
        }

        bool isRemote() const { return m_source != FileSource::Local; }

    protected:
        FileSource m_source;
        StringView m_name;
        StringView m_parent;
    };

    class File : public FileNode
    {
    public:
        File() = default;
        File(StringView fullpath);
        virtual ~File();

    protected:
        UUID m_uniqueId;
        u64  m_size;
        u64  m_bytesTaken;

        Flags<FileAttribute> m_attributes;
    };

    class Directory : public FileNode
    {};

    class Symlink : public FileNode
    {};

    /**
     * @brief A shared directory set
     *
     * A library could be considered as a shared directory set, which maps files
     * in the several specificed directories into one. It does NOT contain any
     * file data, but only mapping records. This is usually used for sharing
     * file views with similar properties, like downloads, documents, etc.
     */
    class Library : public FileNode
    {};
}