#ifndef __SPARSE_HPP__
#define __SPARSE_HPP__

#include <cstdint>
#include <random>
#include <utility>

#include "prime.hpp"

namespace recovery
{

struct sparse1 final
{
    sparse1() : q(d(g)), w3(0), w2(0), w1(0) {}

    sparse1 & operator()(uint16_t index, int8_t update)
    {
        w1 += update;
        w2 += int64_t(index) * update;
        int64_t acc = 1;
        for (uint16_t i = 0; i < index; ++i)
            acc = mp31(acc * q);
        w3 += mp31(acc * update);
    }

    std::pair<uint16_t, int8_t> operator()()
    {
        if (!w1)
        {
            if (w2 || w3)
                return { 2, 0 };
            return { 0, 0 };
        }
        if (w2 % w1)
            return { 2, 0 };
        int64_t j = w2 / w1;
        if (j <= 0)
            return { 2, 0 }
    }
private:
    static constexpr prime::mersenne const & mp31 = prime::mersennes::mersenne31;

    static std::uniform_int_distribution<uint32_t> d;
    static std::mt19937 g;

    int64_t w1, w2, w3;
    uint32_t const q;
};

std::uniform_int_distribution<uint32_t> sparse1::d { 1, mp31.p };
std::mt19937 sparse1::g { std::random_device()() };

}

#endif
