#include <System/UI/ElementProperty.h>

namespace System::UI {
    class Element
    {
    public:
        template <typename T>
        T& GetValue(ElementProperty<T> const&);

        template <typename T>
        void SetValue(ElementProperty<T> const&, T const& value);
    };
}