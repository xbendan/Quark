#pragma once

#include <mixins/std/type_traits.h>
#include <mixins/std/utility.h>

template <typename T>
concept Integral = Std::isIntegral<T>;

template <typename T>
concept FloatingPoint = Std::isFloatingPoint<T>;

template <typename T>
concept Arithmetic = Std::isArithmetic<T>;

template <typename T>
concept Signed = Std::isSigned<T>;

template <typename T>
concept Unsigned = Std::isUnsigned<T>;

template <typename T>
concept Pointer = Std::isPointer<T>;

template <typename T>
concept Reference = Std::isReference<T>;

template <typename T>
concept LValueReference = Std::isLValueReference<T>;

template <typename T>
concept RValueReference = Std::isRValueReference<T>;

template <typename T>
concept Function = Std::isFunction<T>;

template <typename T>
concept Enum = Std::isEnum<T>;

template <typename T>
concept Class = Std::isClass<T>;

template <typename T>
concept Trivial = Std::isTrivial<T>;

template <typename T>
concept StandardLayout = Std::isStandardLayout<T>;

template <typename T>
concept POD = Std::isPOD<T>;

template <typename T>
concept Abstract = Std::isAbstract<T>;

template <typename T>
concept Polymorphic = Std::isPolymorphic<T>;

template <typename T>
concept Final = Std::isFinal<T>;

template <typename T, typename... Ts>
concept Any = (Std::isSame<T, Ts> || ...);

template <typename T>
concept Aggregate = Std::isAggregate<T>;

template <typename T>
concept SignedIntegral = Integral<T> && Signed<T>;

template <typename T>
concept UnsignedIntegral = Integral<T> && Unsigned<T>;

template <typename T>
concept SignedArithmetic = Arithmetic<T> && Signed<T>;

template <typename T>
concept UnsignedArithmetic = Arithmetic<T> && Unsigned<T>;

template <typename T>
concept FloatingPointArithmetic = Arithmetic<T> && FloatingPoint<T>;

template <typename T>
concept Scalar = Arithmetic<T> || Pointer<T> || Reference<T>;

template <typename T>
concept Compound =
    Std::isArray<T> || Function<T> || Enum<T> || Class<T> || __is_union(T);

template <typename T>
concept NonScalar = Compound<T> || Pointer<T>;

template <typename T>
concept NonVoid = !Std::isVoid<T>;

template <typename T>
concept NonConst = !Std::isConst<T>;

template <typename T>
concept NonVolatile = !Std::isVolatile<T>;

template <typename T>
concept NonConstVolatile = NonConst<T> && NonVolatile<T>;

template <typename T>
concept NonConstRef = NonConst<T> && Reference<T>;

template <typename T>
concept NonVolatileRef = NonVolatile<T> && Reference<T>;

template <typename T>
concept NonConstVolatileRef = NonConstVolatile<T> && Reference<T>;

template <typename T>
concept NonConstPointer = NonConst<T> && Pointer<T>;

template <typename T>
concept NonVolatilePointer = NonVolatile<T> && Pointer<T>;

template <typename T>
concept NonConstVolatilePointer = NonConstVolatile<T> && Pointer<T>;

template <typename _E, typename... Args>
concept Invocable = requires(_E f, Args... args) { f(args...); };

template <typename _E, typename... Args>
concept Callable = requires(_E f, Args... args) { f(declval<Args>()...); };

template <typename T, typename... Args>
using InvokeResult =
    decltype(declval<T>()(Std::forward<Args>(declval<Args>())...));

template <typename From, typename To>
concept SameAs = Std::isSame<From, To>;

template <typename From, typename To>
concept ConvertibleTo = Std::isConvertible<From, To>;

template <typename From, typename To>
concept Derived = Std::isDerived<From, To>;

template <typename T>
concept Destructible = requires(T t) { t.~T(); };

template <typename T, typename... Args>
concept Constructible = requires(T t) { ::new T(declval<Args>()...); };

template <typename T, typename U = T>
concept CopyConstructible = Constructible<T, U const&>;

template <typename T, typename U = T>
concept MoveConstructible = Constructible<T, U&&>;

template <typename T, typename Out = T>
concept Comparable = requires(T const& a, T const& b) {
    { a <=> b } -> SameAs<decltype(a <=> b)>;

    { a < b } -> SameAs<bool>;
    { a > b } -> SameAs<bool>;
    { a <= b } -> SameAs<bool>;
    { a >= b } -> SameAs<bool>;
};

template <typename T>
concept Computable = requires(T const& a, T const& b) {
    { a + b } -> SameAs<T>;
    { a - b } -> SameAs<T>;
    { a* b } -> SameAs<T>;
    { a / b } -> SameAs<T>;
};

template <typename T, typename U = T>
concept Equatable = requires(T const& a, U const& b) {
    { a == b } -> SameAs<bool>;
    { a != b } -> SameAs<bool>;
};

template <typename T>
concept Fundamental = Std::isFundamental<T> || requires { T::_Fundamental; };

template <typename...>
inline constexpr usize _indexOf = 0;

template <typename TType, typename TFirst>
inline constexpr usize _indexOf<TType, TFirst> = 0;

template <typename TType, typename TFirst, typename... Rest>
inline constexpr usize _indexOf<TType, TFirst, Rest...> =
    Std::isSame<TType, TFirst> ? 0 : _indexOf<TType, Rest...> + 1;

template <typename TType, typename... Ts>
    requires Any<TType, Ts...>
static consteval usize
indexOf()
{
    return _indexOf<TType, Ts...>;
}

template <typename...>
struct _indexCast;

template <typename Data, typename T>
struct _indexCast<Data, T>
{
    always_inline static auto eval(usize, Data* p, auto func)
    {
        using U = Std::Conditional<Std::isConst<Data>, Data, T>;
        return func(*reinterpret_cast<U*>(p));
    }
};

template <typename Data, typename T, typename... Ts>
struct _indexCast<Data, T, Ts...>
{
    always_inline static auto eval(usize index, Data* p, auto func)
    {
        if (index == 0) {
            using U = Std::Conditional<Std::isConst<Data>, Data, T>;
            return func(*reinterpret_cast<U*>(p));
        }
        return _indexCast<Data, Ts...>::eval(index - 1, p, func);
    }
};

template <typename... Ts>
always_inline static inline auto
indexCast(usize index, auto* p, auto func)
{
    return _indexCast<typename Std::RemoveRef<decltype(*p)>, Ts...>::eval(
        index, p, func);
}