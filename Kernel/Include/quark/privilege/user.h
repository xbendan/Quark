#pragma once

#include <mixins/str/string.h>
#include <mixins/utils/array_list.h>
#include <mixins/utils/date.h>
#include <mixins/utils/uuid.h>

namespace Quark::System::Privilege {

    enum class Level
    {
        None,
        Guest,
        User,
        Administrator,
        System,
        Operator
    };

    struct UserLoginData
    {
        Date _when;
        bool _isRemote;

        bool operator==(const UserLoginData& other) const = default;
    };

    class UserGroup;

    class UserAccount
    {
    public:
        UserAccount() = default;
        UserAccount(StringView name, UserGroup* group)
            : _name(name)
            , _group(group)
            , _id(UUID())
            , _whenCreated(Date())
            , _whenLastLogin(Date())
            , _loginData(UserLoginData())
        {
        }

        constexpr bool operator==(UserAccount const& other) const
        {
            return _name == other._name && _group == other._group;
        }

    private:
        StringView    _name;
        UserGroup*    _group;
        UUID          _id;
        Date          _whenCreated;
        Date          _whenLastLogin;
        UserLoginData _loginData;
    };

    class UserGroup
    {
    public:
        UserGroup() = default;
        UserGroup(StringView name, Level level)
            : m_name(name)
            , m_level(level)
            , m_users(ArrayList<UserAccount>())
        {
        }

        constexpr bool operator==(UserGroup const& other) const
        {
            return m_name == other.m_name && m_level == other.m_level;
        }

    private:
        StringView             m_name;
        Level                  m_level;
        ArrayList<UserAccount> m_users{};
    };
}