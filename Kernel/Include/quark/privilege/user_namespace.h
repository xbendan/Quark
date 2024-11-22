#include <quark/privilege/user.h>

namespace Quark::System::UserManagement {
    /**
     * @brief 用户命名空间
     *
     * 用于隔离每个用户的数据和设定
     * 其中包括了用户设定的：
     * - 运行环境
     * - 系统设定/变量
     * - 权限
     * 等等
     */
    class UserNamespace
    {
        UserID* m_uid;
    };
}