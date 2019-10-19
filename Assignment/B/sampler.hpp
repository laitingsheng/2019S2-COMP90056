#ifndef __SAMPLER_H__
#define __SAMPLER_H__

#include <cmath>
#include <cstdint>

#include "hash.hpp"
#include "recovery.h"

namespace sampler
{

struct l0_insertion final
{
    l0_insertion(uint16_t n, hash::k_universal<uint16_t> && h) noexcept : h(std::move(h)), aref(nullptr), m(uint64_t(n) + 1), n(n) {}

    l0_insertion(l0_insertion &&) noexcept = default;

    l0_insertion & operator+=(uint16_t x) noexcept
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

    operator uint16_t() const noexcept
    {
        return *aref;
    }
private:
    uint64_t n, m;
    uint16_t a, * aref;
    hash::k_universal<uint16_t> h;

    l0_insertion(l0_insertion const &) = delete;
    l0_insertion & operator=(l0_insertion const &) = delete;
    l0_insertion & operator=(l0_insertion &&) = delete;
};

struct l0_general final
{
    l0_general(uint16_t n, double delta, uint8_t t) : h(hash::k_universal_family<uint16_t>((12 * log(1 / delta) + 1) / 2)()), L(ceil(log(n))), r(uint64_t(n) * n * n)
    {
        sparses.reserve(L);
        for (uint8_t i = 0; i < L; ++i)
            sparses.emplace_back(k, delta, t);
    }

    l0_general(l0_general &&) = default;

    l0_general & operator()(uint16_t index, int8_t update)
    {
        uint8_t l = 0;
        double acc = r;
        while (l < L && acc >= h(index) % r)
        {
            sparses[l](index, update);
            ++l;
            acc /= 2;
        }
        return *this;
    }

    operator std::pair<bool, uint16_t>() const
    {
        uint8_t l = 0;
        std::unordered_map<uint16_t, int64_t> output;
        while (l < L)
        {
            std::unordered_map<uint16_t, int64_t> r = sparses[l++];
            if (r.size() > 0)
            {
                output = std::move(r);
                break;
            }
        }
        if (!output.size())
            return { false, 0 };
        uint16_t mi;
        uint64_t mh = std::numeric_limits<uint64_t>::max();
        for (auto const & [k, v] : output)
        {
            uint64_t hr = h(k) % r;
            if (hr < mh)
            {
                mh = hr;
                mi = k;
            }
        }
        return { true, mi };
    }
private:
    uint64_t r;
    uint8_t L;
    hash::k_universal<uint16_t> h;
    std::vector<recovery::sparse_k> sparses;

    l0_general(l0_general const &) = delete;
    l0_general & operator=(l0_general const &) = delete;
    l0_general & operator=(l0_general &&) = delete;
};

}

#endif
