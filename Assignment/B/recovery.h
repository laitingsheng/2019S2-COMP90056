#ifndef __RECOVERY_H__
#define __RECOVERY_H__

#include <cmath>
#include <cstdint>
#include <random>
#include <unordered_map>
#include <utility>
#include <vector>

#include "prime.hpp"
#include "hash.hpp"

namespace recovery
{

// only workds in turnstile
struct sparse_1 final
{
    sparse_1() : q(d(g)), w3(0), w2(0), w1(0) {}

    sparse_1(sparse_1 &&) = default;

    sparse_1 & operator()(uint16_t index, int8_t update)
    {
        w1 += update;
        w2 += int64_t(index) * update;
        int64_t acc = 1;
        for (uint16_t i = 0; i < index; ++i)
            acc = mp(acc * q);
        w3 = mp(w3 + mp(acc * update));
        return *this;
    }

    operator std::pair<uint16_t, uint64_t>() const
    {
        // all f >= 0
        if (!w1)
            return { 0, 0 };
        // there must be more than one existing items with non-zero frequency
        if (w2 % w1)
            return { 2, 0 };
        uint64_t j = w2 / w1;
        // j is limited in range [0, 2^16-1], this means more than items have non-zero frequency
        if (j > std::numeric_limits<uint16_t>::max())
            return { 2, 0 };
        uint64_t acc = 1;
        for (uint16_t i = 0; i < j; ++i)
            acc = mp(acc * q);
        if (mp(acc * mp(w1)) == w3)
            return { j, w1 };
        return { 2, 0 };
    }
private:
    static constexpr prime::mersenne const & mp = prime::mersennes::mersenne31;

    static std::uniform_int_distribution<uint32_t> d;
    static std::mt19937 g;

    sparse_1(sparse_1 const &) = delete;
    sparse_1 & operator=(sparse_1 const &) = delete;
    sparse_1 & operator=(sparse_1 &&) = delete;

    int64_t w1, w2, w3;
    uint32_t q;
};

// it will be nonsense to have q = 0
std::uniform_int_distribution<uint32_t> sparse_1::d { 1, mp.p };
std::mt19937 sparse_1::g { std::random_device()() };

struct sparse_k final
{
    sparse_k(sparse_k &&) = default;

    sparse_k(uint16_t k, double delta, uint8_t t) : l(uint64_t(k) * 2), d(log(k / delta)), k(k)
    {
        hashes.reserve(d);
        records.reserve(d);
        // pairwise
        hash::k_universal_family<uint16_t> kuf(t);
        for (uint64_t i = 0; i < d; ++i)
        {
            hashes.emplace_back(kuf());
            records.emplace_back(l);
        }
    }

    sparse_k & operator()(uint16_t index, int8_t update)
    {
        for (uint64_t i = 0; i < d; ++i)
            records[i][hashes[i](index) % l](index, update);
        return *this;
    }

    operator std::unordered_map<uint16_t, uint64_t>() const
    {
        std::unordered_map<uint16_t, uint64_t> re(k);
        for (auto const & ss : records)
            for (auto const & s : ss)
            {
                std::pair<uint16_t, uint64_t> r = s;
                if (r.second && (!re.count(r.first) || r.second < re[r.first]))
                    re[r.first] = r.second;
                if (re.size() > k)
                    return {};
            }
        return re;
    }
private:
    uint16_t k;
    uint64_t d, l;
    std::vector<hash::k_universal<uint16_t>> hashes;
    std::vector<std::vector<sparse_1>> records;

    sparse_k(sparse_k const &) = delete;
    sparse_k & operator=(sparse_k const &) = delete;
    sparse_k & operator=(sparse_k &&) = delete;
};

}

#endif
