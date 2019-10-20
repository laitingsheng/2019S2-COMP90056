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
    l0_general(uint16_t n, double delta) : h(hash::k_universal_family<uint16_t>(t)()), L(ceil(log(n))), r(uint64_t(n) * n * n), t((k + 1) / 2), k(12 * log(1 / delta))
    {
        sparses.reserve(L);
        for (uint8_t i = 0; i < L; ++i)
            sparses.emplace_back(k, delta, t);
    }

    l0_general(l0_general &&) = default;

    l0_general & operator()(uint16_t index, int8_t update)
    {
        double acc = r;
        uint64_t hv = h(index) % r;
        for (auto & sparse : sparses)
        {
            if (acc < hv)
                break;
            sparse(index, update);
            acc /= 2;
        }
        return *this;
    }

    operator std::pair<bool, uint16_t>() const
    {
        for (auto const & sparse : sparses)
        {
            std::unordered_map<uint16_t, int64_t> re = sparse;
            if (re.size())
                return { true, std::min_element(re.begin(),
                                                re.end(),
                                                [this](auto const & a, auto const & b) -> bool
                                                {
                                                    return h(a.first) % r < h(b.first) % r;
                                                })->first };
        }
        return { false, 0 };
    }
private:
    uint64_t r;
    uint16_t k;
    uint8_t L, t;
    hash::k_universal<uint16_t> h;
    std::vector<recovery::sparse_k> sparses;

    l0_general(l0_general const &) = delete;
    l0_general & operator=(l0_general const &) = delete;
    l0_general & operator=(l0_general &&) = delete;
};

}

#endif
