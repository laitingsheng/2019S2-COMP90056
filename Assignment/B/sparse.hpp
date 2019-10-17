#ifndef __SPARSE_HPP__
#define __SPARSE_HPP__

#include <cstdint>
#include <random>

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
private:
    // Mersenne prime number
    static inline constexpr uint32_t p = (uint32_t(1) << 31) - 1;
    static std::mt19937 g { std::random_device()() };
    static std::uniform_int_distribution<uint32_t> d(1, p);

    int64_t w1, w2, w3;
    uint32_t const q;
};

#endif
