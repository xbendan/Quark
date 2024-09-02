namespace Quark::User {
    enum class Privilege
    {
        None = 0,
        Guest,
        User,
        Administrator,
        System,
        Operator
    };
}