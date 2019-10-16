#ifndef __K_HASH_HPP__
#define __K_HASH_HPP__

#include <cstdint>

#include <functional>
#include <random>

template<typename T>
struct k_hash final
{
    k_hash(uint8_t k) : coeffs(new uint32_t[k]), a(ra(g)), k(k)
    {
        for (uint8_t i = 0; i < k; ++i)
            coeffs[i] = rs(g);
    }

    ~k_hash()
    {
        if (coeffs)
        {
            delete[] coeffs;
        }
    }

    k_hash(k_hash const &) = delete;
    k_hash(k_hash && o) : coeffs(o.coeffs), a(o.a), k(o.k)
    {
        o.coeffs = nullptr;
    }

    k_hash & operator=(k_hash const &) = delete;
    k_hash & operator=(k_hash && o) = delete;

    inline uint64_t operator()(T item) const
    {
        uint64_t x = basic(item), r = x % p, acc = 1, re = 0;
        for (uint8_t i = 0; i < k; ++i)
        {
            re = (re + (coeffs[i] * acc) % p) % p;
            acc = (acc * r) % p;
        }
        return (re + (a * acc) % p) % p;
    }
private:
    static const std::hash<T> basic;
    static inline constexpr uint64_t p = (uint64_t(1) << 31) - 1; // Mersenne prime number

    static std::uniform_int_distribution<uint32_t> ra, rs;
    static std::mt19937 g;

    friend struct k_hash_tester;
    template<typename RA, typename RS>
    k_hash(uint8_t k, RA ra, RS rs) : coeffs(new uint32_t[k]), a(ra(g)), k(k)
    {
        for (uint8_t i = 0; i < k; ++i)
            coeffs[i] = rs(g);
    }

    uint8_t k;
    uint32_t a, * coeffs;
};

template<typename T>
std::uniform_int_distribution<uint32_t> k_hash<T>::ra { 1, p - 1 };
template<typename T>
std::uniform_int_distribution<uint32_t> k_hash<T>::rs { 0, p - 1 };
template<typename T>
std::mt19937 k_hash<T>::g { std::random_device()() };

#endif
