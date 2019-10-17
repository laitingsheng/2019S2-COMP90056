#ifndef __L_HPP__
#define __L_HPP__

#include <cmath>
#include <cstdint>

#include "hash.hpp"

namespace sampler
{

template<typename T>
struct l0_insertion final
{
    l0_insertion(uint64_t n, hash::k_universal<T> h) : h(h), aref(nullptr), m(n + 1), n(n) {}

    l0_insertion(l0_insertion const &) = default;
    l0_insertion(l0_insertion &&) = default;

    l0_insertion & operator+=(T x)
    {
        uint64_t v = h(x) % n;
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
    uint64_t const n;
    uint64_t m;
    T a, *aref;
    hash::k_universal<T> h;

    l0_insertion & operator=(l0_insertion const &) = delete;
    l0_insertion & operator=(l0_insertion &&) = delete;
};

}

#endif
