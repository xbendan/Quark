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
        APPEND,
        CREATE,
        CREATE_IF_ABSENT,
        OPEN,
        OPEN_OR_CREATE,
    };

    enum class FileAttribute : u64
    {
        READONLY = 1,
        HIDDEN,
        ARCHIVE,
        COMPRESSWD,
        ENCRYPTED,
        TEMPORARY,
    };
    MakeFlags$(FileAttribute);

    enum class FileSource
    {
        LOCAL,
        REMOTE,
        CLOUD,
        VIRTUAL,
        DEVICE,
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
        u64 m_size;
        u64 m_bytesTaken;

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