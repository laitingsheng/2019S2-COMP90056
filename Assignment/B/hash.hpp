#ifndef __K_HASH_HPP__
#define __K_HASH_HPP__

#include <cstdint>

#include <functional>
#include <random>

#include "prime.hpp"

namespace hash
{

template<typename T>
struct k_universal final
{
    using ItemType = T;

    k_universal(k_universal const &) = default;
    k_universal(k_universal &&) = default;

    inline constexpr uint64_t operator()(T item) const noexcept
    {
        uint64_t r = mp31(basic(item)), acc = 1, re = 0;
        for (uint32_t i : as)
        {
            re = mp31(re + mp31(mp31(i) * acc));
            acc = mp31(acc * r);
        }
        return mp31(re + mp31(mp31(a) * acc));
    }
private:
    template<typename>
    friend struct k_universal_family;
    friend struct k_universal_tester;

    static constexpr std::hash<T> basic {};
    static constexpr prime::mersenne const & mp31 = prime::mersennes::mersenne31;

    std::uint64_t a;
    std::vector<uint64_t> as;

    k_universal(uint64_t a, std::vector<uint64_t> && as) noexcept : as(as), a(a) {}

    k_universal & operator=(k_universal const &) = delete;
    k_universal & operator=(k_universal &&) = delete;
};

template<typename T>
struct k_universal_family final
{
    using ItemType = T;

    k_universal_family(uint8_t k) : k_universal_family(k, prime::mersennes::mersenne31.p - 1) {}

    k_universal<T> operator()()
    {
        std::vector<uint64_t> as(k);
        for (uint8_t i = 0; i < k; ++i)
            as[i] = asd[i](asg[i]);
        return k_universal<T>(ad(ag), std::move(as));
    }
private:
    friend struct k_universal_tester;

    static std::random_device rd;

    uint8_t k;
    std::uniform_int_distribution<uint64_t> ad;
    std::vector<std::uniform_int_distribution<uint64_t>> asd;
    std::mt19937_64 ag;
    std::vector<std::mt19937_64> asg;

    k_universal_family(uint8_t k, uint32_t max) : ag(rd()), ad(1, max), k(k)
    {
        asd.reserve(k);
        asg.reserve(k);
        for (uint8_t i = 0; i < k; ++i)
        {
            asd.emplace_back(0, max);
            asg.emplace_back(rd());
        }
    }

    k_universal_family(k_universal_family const &) = delete;
    k_universal_family(k_universal_family &&) = delete;

    k_universal_family & operator=(k_universal_family const &) = delete;
    k_universal_family & operator=(k_universal_family &&) = delete;
};

template<typename T>
std::random_device k_universal_family<T>::rd {};

template<typename T, std::enable_if_t<std::is_arithmetic_v<T>, bool> = true>
struct simple_uniform
{
    using ItemType = T;

    simple_uniform(simple_uniform const &) = default;
    simple_uniform(simple_uniform &&) = default;

    inline constexpr uint64_t operator()(T item) const noexcept
    {
        return mp31(mp31(basic(item)) + v);
    }
private:
    template<typename>
    friend struct simple_uniform_family;

    static constexpr std::hash<T> basic {};
    static constexpr prime::mersenne const & mp31 = prime::mersennes::mersenne31;

    uint64_t v;

    simple_uniform(uint64_t v) : v(v) {}

    simple_uniform & operator=(simple_uniform const &) = delete;
    simple_uniform & operator=(simple_uniform &&) = delete;
};

template<typename T>
struct simple_uniform_family
{
    using ItemType = T;

    simple_uniform_family() : ag(rd()), ad(0, prime::mersennes::mersenne31.p - 1) {}

    simple_uniform<T> operator()()
    {
        return simple_uniform<T>(ad(ag));
    }
private:
    static std::random_device rd;

    std::uniform_int_distribution<uint64_t> ad;
    std::mt19937_64 ag;
};

template<typename T>
std::random_device simple_uniform_family<T>::rd {};

}

#endif
