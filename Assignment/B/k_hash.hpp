#ifndef __K_HASH_HPP__
#define __K_HASH_HPP__

#include <cstdint>

#include <functional>
#include <random>

template<typename T>
struct k_hash final
{
    k_hash(uint8_t k) : coeffs(k), a(ra(g)), k(k)
    {
        for (uint8_t i = 0; i < k; ++i)
            coeffs[i] = rs(g);
    }

    ~k_hash() = default;

    k_hash(k_hash const &) = delete;
    k_hash(k_hash &&) = default;

    k_hash & operator=(k_hash const &) = delete;
    k_hash & operator=(k_hash &&) = default;

    inline uint64_t operator()(T item) const
    {
        uint64_t x = basic(item), r = modp(x), acc = 1, re = 0;
        for (uint8_t i = 0; i < k; ++i)
        {
            re = modp(re + modp(coeffs[i] * acc));
            acc = modp(acc * r);
        }
        return modp(re + modp(a * acc));
    }
private:
    static const std::hash<T> basic;
    // Mersenne prime number
    static inline constexpr uint64_t e = 31;
    static inline constexpr uint64_t p = (uint64_t(1) << e) - 1;

    static inline constexpr uint64_t modp(uint64_t n)
    {
        uint64_t t = (n & p) + (n >> e);
        return t < p ? t : t - p;
    }

    static std::uniform_int_distribution<uint32_t> ra, rs;
    static std::mt19937 g;

    friend struct k_hash_tester;
    template<typename RA, typename RS>
    k_hash(uint8_t k, RA ra, RS rs) : coeffs(k), a(ra(g)), k(k)
    {
        for (uint8_t i = 0; i < k; ++i)
            coeffs[i] = rs(g);
    }

    uint8_t k;
    uint32_t a;
    std::vector<uint32_t> coeffs;
};

template<typename T>
std::uniform_int_distribution<uint32_t> k_hash<T>::ra { 1, p - 1 };
template<typename T>
std::uniform_int_distribution<uint32_t> k_hash<T>::rs { 0, p - 1 };
template<typename T>
std::mt19937 k_hash<T>::g { std::random_device()() };

#endif
