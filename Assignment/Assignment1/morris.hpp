#ifndef __MORRIS_HPP__
#define __MORRIS_HPP__

#include <random>

// share across all instantiations of template
static std::uniform_real_distribution<> r;
static std::mt19937_64 g { std::random_device()() };

struct morris_counter final
{
    morris_counter & operator+=(std::size_t freq)
    {
        auto p = 1UL << c;
        for (std::size_t i = 0; i < freq; ++i)
            if (r(g) < 1.0 / p)
            {
                ++c;
                p <<= 1;
            }
        return *this;
    }

    operator std::size_t() const
    {
        return (1UL << c) - 1;
    }
private:
    unsigned char c = 0;
};

#endif
