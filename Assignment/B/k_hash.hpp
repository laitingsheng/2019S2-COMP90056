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
        delete[] coeffs;
        coeffs = nullptr;
        a = 0;
        k = 0;
    }

    k_hash(k_hash const &) = delete;
    k_hash(k_hash &&) = default;

    k_hash & operator=(k_hash const &) = delete;
    k_hash & operator=(k_hash &&) = default;

    inline uint64_t operator()(T item, uint64_t domain) const
    {
        uint64_t x = basic(item), r = x % p, acc = 1, re = 0;
        for (uint8_t i = 0; i < k; ++i)
        {
            re = (re + (coeffs[i] + acc) % p) % p;
            acc = (acc * r) % p;
        }
        return re % domain;
    }
private:
    friend struct k_hash_tester;

    k_hash() = default;

    static const std::hash<T> basic;
    static inline constexpr uint64_t p = (uint64_t(1) << 31) - 1; // Mersenne prime number

    static std::uniform_int_distribution<uint32_t> ra, rs;
    static std::mt19937 g;

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
