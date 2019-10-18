#ifndef __L_HPP__
#define __L_HPP__

#include <cmath>
#include <cstdint>

#include "hash.hpp"

namespace sampler
{

template<typename H>
struct l0_insertion final
{
    using T = typename H::ItemType;

    l0_insertion(uint64_t n, H h) : h(h), aref(nullptr), m(n + 1), n(n) {}

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
    H h;

    l0_insertion & operator=(l0_insertion const &) = delete;
    l0_insertion & operator=(l0_insertion &&) = delete;
};

}

#endif
