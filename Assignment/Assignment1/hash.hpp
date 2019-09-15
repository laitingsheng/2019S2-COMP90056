#ifndef __HASH_HPP__
#define __HASH_HPP__

#include <cstdint>

#include <functional>
#include <random>

template<typename Type>
struct hash final
{
    using item_type = Type;

    hash() : b(rb(g)), a(ra(g)) {}

    // prevent unintentional copy
    hash(hash const &) = delete;
    hash & operator=(hash const &) = delete;

    // force cast back to std::size_t as defined in the STL
    inline uint64_t operator()(Type item, uint64_t domain) const
    {
        return (a * basic(item) + b) % p % domain;
    }
private:
    static constexpr auto basic = std::hash<Type>();

    static inline constexpr uint64_t p = (uint64_t(1) << 31) - 1; // Mersenne prime number
    static std::uniform_int_distribution<uint64_t> ra, rb;
    static std::mt19937_64 g;

    uint64_t const a, b;
};

template<typename Type>
std::uniform_int_distribution<uint64_t> hash<Type>::ra { 1, p - 1 };
template<typename Type>
std::uniform_int_distribution<uint64_t> hash<Type>::rb { 0, p - 1 };
template<typename Type>
std::mt19937_64 hash<Type>::g { std::random_device()() };

#endif
