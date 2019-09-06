#ifndef __HASH_HPP__
#define __HASH_HPP__

#include <functional>
#include <random>

// share across all instantiations of template
static auto constexpr p = 1073741789U;
static std::uniform_int_distribution<unsigned int> random_a(1, p - 1), random_b(0, p - 1);
static std::mt19937_64 generator { std::random_device()() };

template<typename Type>
struct hash final
{
    hash() : b(random_b(generator)), a(random_a(generator)) {}

    // prevent unintentional copy
    hash(hash const &) = delete;
    hash & operator=(hash const &) = delete;

    // force cast back to std::size_t as defined in the STL
    inline std::size_t operator()(Type item, std::size_t domain) const
    {
        return (a * basic(item) + b) % p % domain;
    }
private:
    static constexpr auto basic = std::hash<Type>();

    unsigned int const a, b;
};

#endif
