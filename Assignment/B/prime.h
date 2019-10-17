#ifndef __PRIME_H_
#define __PRIME_H_

#include <cstdint>

namespace prime
{

struct mersenne
{
    uint64_t const s, p;

    inline constexpr uint64_t operator()(uint64_t k) const
    {
        uint64_t i = (k & p) + (k >> s);
        return i < p ? i : i - p;
    }
private:
    friend struct mersennes;
    constexpr mersenne(uint64_t s) : p((uint64_t(1) << s) - 1), s(s) {}

    mersenne(mersenne const &) = delete;
    mersenne(mersenne &&) = delete;

    mersenne & operator=(mersenne const &) = delete;
    mersenne & operator=(mersenne &&) = delete;
};

struct mersennes
{
    static constexpr mersenne mersenne31 { 31 };
};

}

#endif
