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
        UserAccount(String<> name, UserGroup* group)
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
        String<>      _name;
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
        UserGroup(String<> name, Level level)
            : _name(name)
            , _level(level)
            , _users(ArrayList<UserAccount>())
        {
        }

    private:
        String<>               _name;
        Level                  _level;
        ArrayList<UserAccount> _users{};
    };
}