#pragma once

#include <mixins/meta/opt.h>
#include <mixins/std/c++types.h>
#include <mixins/std/type_traits.h>

namespace Qk {
    template <typename TNextFunc>
    struct Iter
    {
        TNextFunc _next;
        using InnerType = decltype(_next());
        using Value     = Std::RemoveCvRef<decltype(*_next())>;

        constexpr Iter(TNextFunc next)
            : _next(next)
        {
        }

        constexpr auto map(auto f)
        {
            auto n = [=, *this] mutable
                -> Optional<decltype(f(*declval<InnerType>()))> {
                auto v = _next();

                if (not v) {
                    return Empty{};
                }

                return f(*v);
            };

            return Iter<decltype(n)>{ n };
        }

        constexpr auto mapi(auto f)
        {
            auto n = [=, *this, index = 0uz] mutable
                -> Optional<decltype(f(*declval<InnerType>(), 1uz))> {
                auto v = _next();
                index++;

                if (not v) {
                    return Empty{};
                }

                return f(*v, index - 1);
            };

            return Iter<decltype(n)>{ n };
        }

        constexpr auto filter(auto f)
        {
            auto n = [=, *this] mutable -> InnerType {
                auto v = _next();
                if (not v) {
                    return Empty{};
                }

                while (not f(*v)) {
                    v = _next();
                    if (not v) {
                        return Empty{};
                    }
                }

                return v;
            };

            return Iter<decltype(n)>{ n };
        }

        constexpr auto reduce(auto init, auto f)
        {
            auto result = init;
            forEach([&](auto v) { result = f(result, v); });
            return result;
        }

        constexpr auto forEach(auto f)
        {
            for (auto item = _next(); item; item = _next()) {
                f(*item);
            }
        }

        constexpr auto find(auto f)
        {
            for (auto item = _next(); item; item = _next()) {
                if (f(*item)) {
                    return *item;
                }
            }

            return Empty{};
        }

        constexpr usize len()
        {
            usize result = 0;
            forEach([&](auto) { result++; });
            return result;
        }

        constexpr auto sum()
        {
            return reduce(Value{}, [](auto& a, auto& b) { return a + b; });
        }

        constexpr bool any() { return _next() != Empty{}; }

        constexpr auto first() { return _next(); }

        constexpr auto first(auto f)
        {
            for (auto item = _next(); item; item = _next()) {
                if (f(*item)) {
                    return item;
                }
            }

            return Empty{};
        }

        constexpr auto last()
        {
            InnerType result = Empty{};
            for (auto item = _next(); item; item = _next()) {
                result = item;
            }
            return result;
        }

        constexpr auto last(auto f)
        {
            InnerType result = Empty{};
            for (auto item = _next(); item; item = _next()) {
                if (f(*item)) {
                    result = item;
                }
            }
            return result;
        }

        constexpr auto skip(usize n)
        {
            for (usize i = 0; i < n; i++) {
                _next();
            }
            return *this;
        }

        constexpr auto cycle(usize n)
        {
            return Iter{ [start = *this, curr = *this, i = 0, n] mutable {
                auto v = curr._next();

                if (not v and i < n) {
                    curr = start;
                    i++;
                    v = curr._next();
                }

                return v;
            } };
        }

        constexpr auto take(usize n)
        {
            return Iter{ [=, *this] mutable {
                if (n == 0) {
                    return Empty{};
                }

                auto v = _next();
                n--;
                return v;
            } };
        }

        constexpr auto prepend(auto v)
        {
            return Iter{ [=, *this, first = true] mutable -> InnerType {
                if (first)
                    return InnerType{ v };

                return _next();
            } };
        }

        constexpr auto append(auto v)
        {
            return Iter{ [=, *this, last = true] mutable -> InnerType {
                auto result = _next();

                if (not result and last) {
                    last = false;
                    return InnerType{ v };
                }

                return result;
            } };
        }

        constexpr auto at(usize n) { return skip(n).first(); }

        constexpr bool any(auto f) { return find(f) != Empty{}; }

        constexpr bool all(auto f)
        {
            for (auto item = _next(); item; item = _next()) {
                if (not f(*item)) {
                    return false;
                }
            }

            return true;
        }

        constexpr auto min()
        {
            InnerType result = Empty{};

            for (auto item = _next(); item; item = _next()) {
                if (not result or (*item < *result)) {
                    result = item;
                }
            }

            return result;
        }

        constexpr auto max()
        {
            InnerType result = Empty{};

            for (auto item = _next(); item; item = _next()) {
                if (not result or (*item > *result)) {
                    result = item;
                }
            }

            return result;
        }

        constexpr auto avg() -> InnerType
        {
            InnerType result = Empty{};
            usize     count  = 0;

            for (auto item = _next(); item; item = _next()) {
                if (not result) {
                    result = item;
                } else {
                    result = *result + *item;
                }
                count++;
            }

            if (count == 0) {
                return Empty{};
            }

            return *result / count;
        }

        struct It
        {
            InnerType _curr;
            Iter      _iter;

            constexpr auto& operator*() { return *_curr; }

            constexpr auto operator++()
            {
                _curr = _iter._next();
                return *this;
            }

            constexpr bool operator!=(Empty) { return _curr != Empty{}; }
        };

        constexpr It begin() { return { _next(), *this }; }

        constexpr Empty end() { return Empty{}; }

        template <typename C>
        constexpr auto collect(C& c)
        {
            forEach([&](auto v) { c.pushBack(v); });
        }

        template <typename C>
        constexpr auto collect(C& c, auto f)
        {
            forEach([&](auto v) { c.pushBack(f(v)); });
        }

        template <typename C>
        constexpr auto collect()
        {
            C c;
            collect(c);
            return c;
        }

        template <typename C>
        constexpr auto collect(auto f)
        {
            C c;
            collect(c, f);
            return c;
        }
    };

    template <typename T>
    constexpr auto iterSingle(T value)
    {
        return Iter<Empty>([value, end = false] mutable {
            if (end) {
                return Empty{};
            }

            end = true;
            return value;
        });
    }

    template <typename T>
    constexpr auto iterRepeat(T value, usize count)
    {
        return Iter([value, count] mutable -> Optional<T> {
            if (count == 0) {
                return Empty{};
            }

            count--;
            return value;
        });
    }
}