#ifndef __PRIME_HPP_
#define __PRIME_HPP_

#include <cstdint>
#include <type_traits>

namespace prime
{

struct mersenne final
{
    uint64_t const s, p;

    template<typename T, std::enable_if_t<std::is_unsigned_v<T>, bool> = true>
    inline constexpr uint64_t operator()(T k) const noexcept
    {
        uint64_t i = k & p;
        i += k >> s;
        return i < p ? i : i - p;
    }

    template<typename T, std::enable_if_t<std::is_signed_v<T>, bool> = true>
    inline constexpr uint64_t operator()(T k) const noexcept
    {
        return k >= 0 ? operator()(uint64_t(k)) : p - operator()(uint64_t(-k));
    }
private:
    friend struct mersennes;
    constexpr mersenne(uint64_t s) noexcept : p((uint64_t(1) << s) - 1), s(s) {}

    mersenne(mersenne const &) = delete;
    mersenne(mersenne &&) = delete;

    mersenne & operator=(mersenne const &) = delete;
    mersenne & operator=(mersenne &&) = delete;
};

struct mersennes
{
    static constexpr mersenne mersenne7 { 7 };
    static constexpr mersenne mersenne13 { 13 };
    static constexpr mersenne mersenne17 { 17 };
    static constexpr mersenne mersenne31 { 31 };
};

}

#endif
