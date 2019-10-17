#ifndef __K_HASH_HPP__
#define __K_HASH_HPP__

#include <cstdint>

#include <functional>
#include <random>

template<typename T>
struct k_hash final
{
    k_hash(uint8_t k) : k_hash(k, p - 1) {}

    k_hash(k_hash const &) = delete;
    k_hash(k_hash && o) = default;

    k_hash & operator=(k_hash const &) = delete;
    k_hash & operator=(k_hash && o) = default;

    inline uint64_t operator()(T item) const
    {
        uint64_t x = basic(item), r = x % p, acc = 1, re = 0;
        for (auto c : coeffs)
        {
            re = (re + (c * acc) % p) % p;
            acc = (acc * r) % p;
        }
        return (re + (a * acc) % p) % p;
    }
private:
    static const std::hash<T> basic;
    // Mersenne prime number
    static inline constexpr uint64_t p = (uint64_t(1) << 31) - 1;

    friend struct k_hash_tester;
    k_hash(uint8_t k, uint32_t max) : coeffs(k), k(k)
    {
        std::random_device rd;
        std::mt19937 g(rd());
        a = std::uniform_int_distribution<uint32_t>(1, max)(g);
        for (auto & c : coeffs)
            c = std::uniform_int_distribution<uint32_t>(0, max)(g);
    }

    uint8_t k;
    uint32_t a;
    std::vector<uint32_t> coeffs;
};

#endif
