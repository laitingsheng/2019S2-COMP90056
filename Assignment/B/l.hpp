#ifndef __L_HPP__
#define __L_HPP__

#include <cmath>
#include <cstdint>

#include "k_hash.hpp"

template<typename T>
struct l0_insertion_sampler final
{
    l0_insertion_sampler(std::uint64_t n, double epsilon) : aref(nullptr), h(std::uint64_t(floor(log(1 / epsilon)))), m(n + 1), n(n) {}

    l0_insertion_sampler & operator+=(T x)
    {
        std::uint64_t v = h(x) % n;
        if (v < m)
        {
            a = x;
            aref = &a;
            m = v;
        }
        return *this;
    }

    operator T() const
    {
        return *aref;
    }
private:
    std::uint64_t n, m;
    k_hash<T> h;
    T a, *aref;
};

#endif
