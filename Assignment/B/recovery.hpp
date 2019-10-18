#ifndef __SPARSE_HPP__
#define __SPARSE_HPP__

#include <cstdint>
#include <random>
#include <utility>
#include <vector>

#include "prime.hpp"

namespace recovery
{

struct sparse_1 final
{
    sparse_1() : q(d(g)), w3(0), w2(0), w1(0) {}

    sparse_1 & operator()(uint16_t index, int8_t update)
    {
        w1 += update;
        w2 += int64_t(index) * update;
        int64_t acc = 1;
        for (uint16_t i = 0; i < index; ++i)
            acc = mp31(acc * q);
        w3 = mp31(w3 + mp31(acc * update));
    }

    std::pair<uint16_t, int8_t> operator()()
    {
        // all f >= 0
        if (!w1)
            return { 0, 0 };
        // there exists more than one items with non-zero frequency
        if (w2 % w1)
            return { 2, 0 };
        uint16_t j = w2 / w1;
        uint64_t acc = 1;
        for (uint16_t i = 0; i < index; ++i)
            acc = mp31(acc * q);
        if (mp31(acc * mp31(w1)) == w3)
            return { j, w1 };
        return { 2, 0 };
    }
private:
    static constexpr prime::mersenne const & mp31 = prime::mersennes::mersenne31;

    static std::uniform_int_distribution<uint32_t> d;
    static std::mt19937 g;

    sparse_1(sparse_1 const &) = delete;
    sparse_1(sparse_1 &&) = delete;

    sparse_1 & operator=(sparse_1 const &) = delete;
    sparse_1 & operator=(sparse_1 &&) = delete;

    uint64_t w1, w2, w3;
    uint32_t const q;
};

// it will be nonsense to have q = 0
std::uniform_int_distribution<uint32_t> sparse_1::d { 1, mp31.p };
std::mt19937 sparse_1::g { std::random_device()() };

struct sparse_k final
{
    sparse_k(uint16_t k, double delta) {}
private:
    std::vector<hash::k_universal<uint16_t>> hashes;
    std::vector<std::vector<sparse_1>> records;
};

}

#endif
