#ifndef __MORRIS_HPP__
#define __MORRIS_HPP__

#include <cstdint>

#include <random>

struct morris_counter final
{
    inline morris_counter & operator+=(uint64_t freq)
    {
        auto p = uint64_t(1) << c;
        for (uint64_t i = 0; i < freq; ++i)
            if (r(g) < 1.0 / p)
            {
                ++c;
                p <<= 1;
            }
        return *this;
    }

    inline operator std::size_t() const
    {
        return (1UL << c) - 1;
    }
private:
    static std::uniform_real_distribution<> r;
    static std::mt19937_64 g;

    uint8_t c = 0;
};

std::uniform_real_distribution<> morris_counter::r {};
std::mt19937_64 morris_counter::g { std::random_device()() };

#endif
