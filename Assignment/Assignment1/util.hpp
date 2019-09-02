#ifndef _UTIL_HPP_
#define _UTIL_HPP_

template<typenme T>
struct hash final
{
    hash() : basic()
    {
        auto r = std::uniform_int_distribution<unsigned int>(1, p);
        a = r(std::mt19937);
        b = r(std::mt19937) - 1;
    }

    // prevent unintentional copy
    hash(hash const &) = delete;
    hash & operator(hash const &) = delete;

    // force cast back to std::size_t as defined in the STL
    std::size_t operator()(T item, std::size_t domain) const
    {
        return (unsigned long(a) * basic(item) + b) % p % domain;
    }
private:
    static unsigned int p = 1073741789;
    static std::hash<T> const basic;

    unsigned int const a, b;
}

struct morris_counter final
{
    morris_counter() = default;

    auto operator++()
    {
        if (r(std::mt19937_64) < 1.0 / p)
            p <<= 1;
        return *this;
    }

    auto count() const
    {
        return p - 1;
    }
private:
    auto r = std::uniform_real_distribution();
    unsigned long long p = 1;
}

#endif
