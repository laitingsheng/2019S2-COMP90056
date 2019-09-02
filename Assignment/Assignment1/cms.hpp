#ifndef _CMS_HPP_
#define _CMS_HPP_

#include <cmath>

template<typename T>
struct cms_default
{
    explicit cms_default(double epsilon, double delta)
    {
        w = ceil(2 / epsilon / epsilon);
        d = ceil(log(1 / delta));
        hashes = new hash[d];
        init()
    }

    virtual void update(T item, std::size_t freq)
    {
        for (int i = 0; i < d; ++i)
            C[i * w + hashes[i](item)] += freq;
    }

    virtual auto query(T item)
    {
        auto m = C[hashes[0](item)];
        for (int i = 1; i < d; ++i)
        {
            auto c = C[i * w + hashes[i](item)];
            if (c < m)
                m = c;
        }
        return m;
    }

    virtual ~cms_default() final
    {
        delete[] hashes;
        delete[] C;
    }
protected:
    unsigned long const w, d;
    hash<T> const * const hashes;
    unsigned long * const C;

    virtual inline void init()
    {
        C = new unsigned long[d * w]();
    }
}

template<typename T>
struct cms_conservative final : public cms_default<T>
{
    explicit cms_conservative(double epsilon, double delta) : cms_default(epsilon, delta) {}

    virtual void update(T item, std::size_t freq) override
    {
        auto f = query(item) + freq;
        for (int i = 0; i < d; ++i)
        {
            auto & c = C[i * w + hashes[i](item)];
            if (f > c)
                c = f;
        }
    }
}

template<typename T>
struct cms_morris final : public cms_default<T>
{
    explicit cms_morris(double epsilon, double delta) : cms_default(epsilon, delta) {}

    virtual void update(T item, std::size_t freq) override
    {
        for (int i = 0; i < d; ++i)
        {
            auto & p = C[i * w + hashes[i](item)];
            auto r = std::uniform_real_distribution();
            for (int j = 0; j < freq; ++j)
                if (r(std::mt19937_64) < 1.0 / p)
                    p <<= 1;
        }
    }

    virtual void query(T item) override
    {
        return cms_default::query(item) - 1;
    }
protected:
    virtual inline void init() override
    {
        auto t = d * w;
        C = new unsigned long[t];
        for (int i = 0; i < t; ++i)
            C[i] = 1;
    }
}

#endif
