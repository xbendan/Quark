#pragma once

#include <mixins/std/string.h>
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
        UserAccount(string name, UserGroup* group)
            : _name(name)
            , _group(group)
            , _id(UUID())
            , _whenCreated(Date())
            , _whenLastLogin(Date())
            , _loginData(UserLoginData())
        {
        }

        constexpr UserAccount& operator=(const UserAccount& other) = default;

        constexpr bool operator==(const UserAccount& other) const = default;

    private:
        string        _name;
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
        UserGroup(string name, Level level)
            : m_name(name)
            , m_level(level)
            , m_users(ArrayList<UserAccount>())
        {
        }

    private:
        string                 m_name;
        Level                  m_level;
        ArrayList<UserAccount> m_users{};
    };
}