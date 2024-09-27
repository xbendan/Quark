#pragma once

#include <mixins/std/c++types.h>
#include <mixins/std/decl.h>

namespace Std {
    template <class T, T t>
    struct Constant
    {
        static constexpr T value = t;
        using ValueType          = T;
        using Type               = Constant<T, t>;

        constexpr operator ValueType() const noexcept { return value; }
        constexpr ValueType operator()() const noexcept { return value; }
    };

    using TrueType  = Constant<bool, true>;
    using FalseType = Constant<bool, false>;

    template <typename T>
    struct TypeIdentity
    {
        using Type = T;
    };

    template <typename T, unsigned = sizeof(T)>
    constexpr TrueType isCompleteOrUnbounded(TypeIdentity<T>) noexcept
    {
        return TrueType();
    }

    template <typename T>
    struct _isIntegral : FalseType
    {};

    template <>
    struct _isIntegral<char> : TrueType
    {};

    template <>
    struct _isIntegral<short> : TrueType
    {};

    template <>
    struct _isIntegral<int> : TrueType
    {};

    template <>
    struct _isIntegral<long> : TrueType
    {};

    template <>
    struct _isIntegral<long long> : TrueType
    {};

    template <>
    struct _isIntegral<unsigned char> : TrueType
    {};

    template <>
    struct _isIntegral<unsigned short> : TrueType
    {};

    template <>
    struct _isIntegral<unsigned int> : TrueType
    {};

    template <>
    struct _isIntegral<unsigned long> : TrueType
    {};

    template <>
    struct _isIntegral<unsigned long long> : TrueType
    {};

    template <typename T>
    inline constexpr bool isIntegral = _isIntegral<T>::value;

    template <typename T>
    struct _isFloatingPoint : FalseType
    {};

    template <>
    struct _isFloatingPoint<float> : TrueType
    {};

    template <>
    struct _isFloatingPoint<double> : TrueType
    {};

    template <>
    struct _isFloatingPoint<long double> : TrueType
    {};

    template <typename T>
    inline constexpr bool isFloatingPoint = _isFloatingPoint<T>::value;

    template <typename T>
    struct _isArithmetic : Constant<bool, isIntegral<T> || isFloatingPoint<T>>
    {};

    template <typename T>
    inline constexpr bool isArithmetic = _isArithmetic<T>::value;

    template <typename T>
    struct _isPointer : FalseType
    {};

    template <typename T>
    struct _isPointer<T*> : TrueType
    {};

    template <typename T>
    inline constexpr bool isPointer = _isPointer<T>::value;

    template <typename T>
    struct _isReference : FalseType
    {};

    template <typename T>
    struct _isReference<T&> : TrueType
    {};

    template <typename T>
    struct _isReference<T&&> : TrueType
    {};

    template <typename T>
    inline constexpr bool isReference = _isReference<T>::value;

    template <typename T>
    struct _isLValueReference : FalseType
    {};

    template <typename T>
    struct _isLValueReference<T&> : TrueType
    {};

    template <typename T>
    inline constexpr bool isLValueReference = _isLValueReference<T>::value;

    template <typename T>
    struct _isRValueReference : FalseType
    {};

    template <typename T>
    struct _isRValueReference<T&&> : TrueType
    {};

    template <typename T>
    inline constexpr bool isRValueReference = _isRValueReference<T>::value;

    template <typename T>
    struct _removeConst
    {
        using Type = T;
    };

    template <typename T>
    struct _removeConst<const T>
    {
        using Type = T;
    };

    template <typename T>
    using RemoveConst = typename _removeConst<T>::Type;

    template <typename T>
    struct _removeVolatile
    {
        using Type = T;
    };

    template <typename T>
    struct _removeVolatile<volatile T>
    {
        using Type = T;
    };

    template <typename T>
    using RemoveVolatile = typename _removeVolatile<T>::Type;

    template <typename T>
    struct _removeCv
    {
        using Type = RemoveVolatile<RemoveConst<T>>;
    };

    template <typename T>
    using RemoveCv = typename _removeCv<T>::Type;

    template <typename T>
    struct _removePointer
    {
        using Type = T;
    };

    template <typename T>
    struct _removePointer<T*>
    {
        using Type = T;
    };

    template <typename T>
    using RemovePointer = typename _removePointer<T>::Type;

    template <typename T>
    struct _removeRef
    {
        using Type = T;
    };

    template <typename T>
    struct _removeRef<T&>
    {
        using Type = T;
    };

    template <typename T>
    struct _removeRef<T&&>
    {
        using Type = T;
    };

    template <typename T>
    using RemoveRef = typename _removeRef<T>::Type;

    template <typename T>
    struct _removeCvRef
    {
        using Type = RemoveCv<RemoveRef<T>>;
    };

    template <typename T>
    using RemoveCvRef = typename _removeCvRef<T>::Type;

    template <typename T>
    struct _addConst
    {
        using Type = const T;
    };

    template <typename T>
    using AddConst = typename _addConst<T>::Type;

    template <typename T>
    struct _addVolatile
    {
        using Type = volatile T;
    };

    template <typename T>
    using AddVolatile = typename _addVolatile<T>::Type;

    template <typename T>
    struct _addCV
    {
        using Type = AddVolatile<AddConst<T>>;
    };

    template <typename T>
    using AddCV = typename _addCV<T>::Type;

    template <typename T>
    struct _addPointer
    {
        using Type = T*;
    };

    template <typename T>
    using AddPointer = typename _addPointer<T>::Type;

    template <typename T>
    struct _addLValueReference
    {
        using Type = T&;
    };

    template <typename T>
    using AddLValueReference = typename _addLValueReference<T>::Type;

    template <typename T>
    struct _addRValueReference
    {
        using Type = T&&;
    };

    template <typename T>
    using AddRValueReference = typename _addRValueReference<T>::Type;

    template <typename T>
    struct _isMemberPointer : FalseType
    {};

    template <typename T, typename U>
    struct _isMemberPointer<T U::*> : TrueType
    {};

    template <typename T>
    inline constexpr bool isMemberPointer =
        _isMemberPointer<RemoveCv<T>>::value;

    template <typename T>
    inline constexpr bool isEnum = Constant<bool, __is_enum(T)>::value;

    template <typename T>
    inline constexpr bool isUnion = Constant<bool, __is_union(T)>::value;

    template <typename T>
    inline constexpr bool isClass = Constant<bool, __is_class(T)>::value;

    template <typename>
    struct _isFunction : FalseType
    {};

    template <typename Ret, typename... Args>
    struct _isFunction<Ret(Args...)> : TrueType
    {};

    template <typename... Ts>
    inline constexpr bool isFunction = _isFunction<Ts...>::value;

    template <typename T>
    struct _isArray : FalseType
    {};

    template <typename T>
    struct _isArray<T[]> : TrueType
    {};

    template <typename T, usize N>
    struct _isArray<T[N]> : TrueType
    {};

    template <typename T>
    inline constexpr bool isArray = _isArray<T>::value;

    template <typename T>
    struct _isVoid : FalseType
    {};

    template <>
    struct _isVoid<void> : TrueType
    {};

    template <typename T>
    inline constexpr bool isVoid = _isVoid<T>::value;

    template <typename T>
    struct _isConst : FalseType
    {};

    template <typename T>
    struct _isConst<const T> : TrueType
    {};

    template <typename T>
    inline constexpr bool isConst = _isConst<T>::value;

    template <typename T>
    struct _isVolatile : FalseType
    {};

    template <typename T>
    struct _isVolatile<volatile T> : TrueType
    {};

    template <typename T>
    inline constexpr bool isVolatile = _isVolatile<T>::value;

    template <typename T>
    struct _isTrivial : Constant<bool, __is_trivial(T)>
    {
        static_assert(
            isCompleteOrUnbounded(TypeIdentity<T>()),
            "template argument must be a complete class or an unbounded array");
    };

    template <typename T>
    inline constexpr bool isTrivial = _isTrivial<T>::value;

    template <typename T>
    struct _isTriviallyCopyable : Constant<bool, __is_trivially_copyable(T)>
    {
        static_assert(
            isCompleteOrUnbounded(TypeIdentity<T>()),
            "template argument must be a complete class or an unbounded array");
    };

    template <typename T>
    inline constexpr bool isTriviallyCopyable = _isTriviallyCopyable<T>::value;

    template <typename T>
    struct _isStandardLayout : Constant<bool, __is_standard_layout(T)>
    {
        static_assert(
            isCompleteOrUnbounded(TypeIdentity<T>()),
            "template argument must be a complete class or an unbounded array");
    };

    template <typename T>
    inline constexpr bool isStandardLayout = _isStandardLayout<T>::value;

    template <typename T>
    struct _isPOD : Constant<bool, __is_pod(T)>
    {
        static_assert(
            isCompleteOrUnbounded(TypeIdentity<T>()),
            "template argument must be a complete class or an unbounded array");
    };

    template <typename T>
    inline constexpr bool isPOD = _isPOD<T>::value;

    template <typename T>
    struct _isLiteralType : Constant<bool, __is_literal_type(T)>
    {
        static_assert(
            isCompleteOrUnbounded(TypeIdentity<T>()),
            "template argument must be a complete class or an unbounded array");
    };

    template <typename T>
    inline constexpr bool isLiteralType = _isLiteralType<T>::value;

    template <typename T>
    struct _isEmpty : Constant<bool, __is_empty(T)>
    {
        static_assert(
            isCompleteOrUnbounded(TypeIdentity<T>()),
            "template argument must be a complete class or an unbounded array");
    };

    template <typename T>
    inline constexpr bool isEmpty = _isEmpty<T>::value;

    template <typename T>
    struct _isPolymorphic : Constant<bool, __is_polymorphic(T)>
    {
        static_assert(
            isCompleteOrUnbounded(TypeIdentity<T>()),
            "template argument must be a complete class or an unbounded array");
    };

    template <typename T>
    inline constexpr bool isPolymorphic = _isPolymorphic<T>::value;

    template <typename T>
    struct _isAbstract : Constant<bool, __is_abstract(T)>
    {
        static_assert(
            isCompleteOrUnbounded(TypeIdentity<T>()),
            "template argument must be a complete class or an unbounded array");
    };

    template <typename T>
    inline constexpr bool isAbstract = _isAbstract<T>::value;

    template <typename T>
    struct _isFinal : Constant<bool, __is_final(T)>
    {
        static_assert(
            isCompleteOrUnbounded(TypeIdentity<T>()),
            "template argument must be a complete class or an unbounded array");
    };

    template <typename T>
    inline constexpr bool isFinal = _isFinal<T>::value;

    template <typename T>
    struct _isAggregate : Constant<bool, __is_aggregate(T)>
    {
        static_assert(
            isCompleteOrUnbounded(TypeIdentity<T>()),
            "template argument must be a complete class or an unbounded array");
    };

    template <typename T>
    inline constexpr bool isAggregate = _isAggregate<T>::value;

    template <typename T>
        requires isArithmetic<T>
    struct _isSigned : Constant<bool, T(-1) < T(0)>
    {};

    template <typename T>
    inline constexpr bool isSigned = _isSigned<T>::value;

    template <typename T>
        requires isArithmetic<T>
    struct _isUnsigned : Constant<bool, T(0) < T(-1)>
    {};

    template <typename T>
    inline constexpr bool isUnsigned = _isUnsigned<T>::value;

    template <typename T>
    struct _isBoundedArray : FalseType
    {};

    template <typename T, usize N>
    struct _isBoundedArray<T[N]> : TrueType
    {};

    template <typename T>
    inline constexpr bool isBoundedArray = _isBoundedArray<T>::value;

    template <typename T>
    struct _isUnboundedArray : FalseType
    {};

    template <typename T>
    struct _isUnboundedArray<T[]> : TrueType
    {};

    template <typename T>
    inline constexpr bool isUnboundedArray = _isUnboundedArray<T>::value;

    template <typename T>
    struct _isFunctionPointer : FalseType
    {};

    template <typename Ret, typename... Args>
    struct _isFunctionPointer<Ret (*)(Args...)> : TrueType
    {};

    template <typename T>
    inline constexpr bool isFunctionPointer = _isFunctionPointer<T>::value;

    template <typename T>
    struct _isFundamental : Constant<bool, isArithmetic<T> || isVoid<T>>
    {};

    template <typename T>
    inline constexpr bool isFundamental = _isFundamental<T>::value;

    template <typename T>
    struct _isScalar
        : Constant<bool,
                   isArithmetic<T> || isPointer<T> || isMemberPointer<T> ||
                       isEnum<T> || isUnboundedArray<T>>
    {};

    template <typename T>
    inline constexpr bool isScalar = _isScalar<T>::value;

    template <typename T>
    struct _isCompound
        : Constant<bool,
                   isReference<T> || isArray<T> || isFunction<T> || isClass<T>>
    {};

    template <typename T>
    inline constexpr bool isCompound = _isCompound<T>::value;

    template <typename T>
    struct _isObject
        : Constant<bool, isScalar<T> || isCompound<T> || isMemberPointer<T>>
    {};

    template <typename T>
    inline constexpr bool isObject = _isObject<T>::value;

    // template <typename T>
    // struct _isTriviallyDestructible
    //     : Constant<bool, __is_trivially_destructible(T)>
    // {};

    // template <typename T>
    // inline constexpr bool isTriviallyDestructible =
    //     _isTriviallyDestructible<T>::value;

    template <typename T, typename... Args>
    struct _isConstructible : Constant<bool, __is_constructible(T, Args...)>
    {};

    template <typename T, typename... Args>
    inline constexpr bool isConstructible = _isConstructible<T, Args...>::value;

    template <typename T, typename U>
    struct _isAssignable : Constant<bool, __is_assignable(T, U)>
    {};

    template <typename T, typename U>
    inline constexpr bool isAssignable = _isAssignable<T, U>::value;

    // template <typename T>
    // struct _isDestructible : Constant<bool, __is_destructible(T)>
    // {};

    // template <typename T>
    // inline constexpr bool isDestructible = _isDestructible<T>::value;

    template <typename T>
    struct _isTriviallyConstructible
        : Constant<bool, __is_trivially_constructible(T)>
    {};

    template <typename T>
    inline constexpr bool isTriviallyConstructible =
        _isTriviallyConstructible<T>::value;

    template <typename T>
    struct _isNothrowConstructible
        : Constant<bool, __is_nothrow_constructible(T)>
    {};

    template <typename T>
    inline constexpr bool isNothrowConstructible =
        _isNothrowConstructible<T>::value;

    template <typename T, typename U>
    struct _isTriviallyAssignable
        : Constant<bool, __is_trivially_assignable(T, U)>
    {};

    template <typename T, typename U>
    inline constexpr bool isTriviallyAssignable =
        _isTriviallyAssignable<T, U>::value;

    template <typename T, typename U>
    struct _isNothrowAssignable : Constant<bool, __is_nothrow_assignable(T, U)>
    {};

    template <typename T, typename U>
    inline constexpr bool isNothrowAssignable =
        _isNothrowAssignable<T, U>::value;

    // template <typename T>
    // struct _isNothrowDestructible : Constant<bool,
    // __is_nothrow_destructible(T)>
    // {};

    // template <typename T>
    // inline constexpr bool isNothrowDestructible =
    //     _isNothrowDestructible<T>::value;

    template <typename From, typename To>
    struct _isSame : FalseType
    {};

    template <typename T>
    struct _isSame<T, T> : TrueType
    {};

    template <typename From, typename To>
    inline constexpr bool isSame = _isSame<From, To>::value;

    namespace _ {
        template <class T>
        auto tReturnable(int) -> decltype(void(static_cast<T (*)()>(nullptr)),
                                          TrueType{});
        template <class>
        auto tReturnable(...) -> FalseType;

        template <class From, class To>
        auto tImplicitlyConvertible(int)
            -> decltype(void(declval<void (&)(To)>()(declval<From>())),
                        TrueType{});

        template <class, class>
        auto tImplicitlyConvertible(...) -> FalseType;
    }

    template <typename From, typename To>
    struct _isConvertible
        : Constant<bool,
                   (decltype(_::tReturnable<To>(0))::value &&
                    decltype(_::tImplicitlyConvertible<From, To>(0))::value) ||
                       (isVoid<From> && isVoid<To>)>
    {};

    template <typename From, typename To>
    inline constexpr bool isConvertible = _isConvertible<From, To>::value;

    // template <typename From, typename To>
    // struct _isConvertible : public Constant<bool, __is_convertible(From, To)>
    // {};

    // template <typename From, typename To>
    // inline constexpr bool isConvertible = _isConvertible<From, To>::value;

    template <typename From, typename To>
    struct _isDerived : Constant<bool, __is_base_of(From, To)>
    {};

    template <typename From, typename To>
    inline constexpr bool isDerived = _isDerived<From, To>::value;

    template <bool B, typename T = void>
    struct _enableIf
    {};

    template <typename T>
    struct _enableIf<true, T>
    {
        using Type = T;
    };

    template <bool B, typename T = void>
    using EnableIf = typename _enableIf<B, T>::Type;

    template <bool B, class T, class _E>
    struct _conditional
    {
        using Type = T;
    };

    template <class T, class _E>
    struct _conditional<false, T, _E>
    {
        using Type = _E;
    };

    template <bool B, class T, class _E>
    using Conditional = typename _conditional<B, T, _E>::Type;

    template <typename T>
        requires isEnum<T>
    struct _underlyingType
    {
        using Type = __underlying_type(T);
    };

    template <typename T>
    using UnderlyingType = typename _underlyingType<T>::Type;
}