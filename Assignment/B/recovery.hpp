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
            acc = acc * q % p;
        w3 += acc * update % p;
    }

    std::pair<uint16_t, int8_t> operator()()
    {
        if (!w1)
            return {0, 0};
        int64_t re = 1;
    }
private:
    static std::mt19937 g { std::random_device()() };
    static std::uniform_int_distribution<uint32_t> d(1, prime::mersennes::mersenne31.p);

    int64_t w1, w2, w3;
    uint32_t const q;
};

}

#endif
