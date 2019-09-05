#include <iostream>
#include <random>

template<typename T>
struct hash final
{
    hash() : b(r(g) - 1), a(r(g)) {}

    // prevent unintentional copy
    hash(hash const &) = delete;
    hash & operator=(hash const &) = delete;

    // force cast back to std::size_t as defined in the STL
    std::size_t operator()(T item, unsigned long domain) const
    {
        return (static_cast<unsigned long>(a) * basic(item) + b) % p % domain;
    }
private:
    static constexpr auto p = 1073741789U;
    static constexpr auto basic = std::hash<T>();
    static std::uniform_int_distribution<unsigned int> r;
    static std::mt19937 g;

    unsigned int const a, b;
};

template<typename T>
std::uniform_int_distribution<unsigned int> hash<T>::r(1, hash<T>::p);

template<typename T>
std::mt19937 hash<T>::g = std::mt19937();

template<typename T, typename C>
struct cms
{
    cms(double epsilon, double delta) : counters(new C[w * d]()),
                                        hashes(new hash<T>[d]()),
                                        d(ceil(log(1 / delta))),
                                        w(ceil(2 / epsilon / epsilon)) {}

    cms(cms const &) = delete;
    cms & operator=(cms const &) = delete;

    virtual std::size_t memory_allocated() final
    {
        return sizeof(hash<T>) * d + sizeof(C) * d * w;
    }

    virtual ~cms()
    {
        delete[] hashes;
        delete[] counters;
    }

    virtual void update(T item, std::size_t freq) = 0;
    virtual unsigned long query(T item) = 0;
protected:
    unsigned long const w, d;
    hash<T> const * const hashes;
    C * const counters;
};

template<typename T>
struct cms_default : public cms<T, unsigned long>
{
    explicit cms_default(double epsilon, double delta) : cms<T, unsigned long>(epsilon, delta) {}

    virtual void update(T item, std::size_t freq)
    {
        for (auto i = 0UL; i < this->d; ++i)
            this->counters[i * this->w + this->hashes[i](item, this->w)] += freq;
    }

    virtual unsigned long query(T item)
    {
        auto m = this->counters[this->hashes[0](item, this->w)];
        for (auto i = 1UL; i < this->d; ++i)
        {
            auto c = this->counters[i * this->w + this->hashes[i](item, this->w)];
            if (c < m)
                m = c;
        }
        return m;
    }
};

template<typename T>
struct cms_conservative final : public cms_default<T>
{
    explicit cms_conservative(double epsilon, double delta) : cms_default<T>(epsilon, delta) {}

    virtual void update(T item, std::size_t freq) override
    {
        auto f = this->query(item) + freq;
        for (auto i = 0UL; i < this->d; ++i)
        {
            auto & c = this->counters[i * this->w + this->hashes[i](item, this->w)];
            if (f > c)
                c = f;
        }
    }
};

template<typename T>
struct cms_morris final : public cms<T, unsigned char>
{
    explicit cms_morris(double epsilon, double delta) : cms<T, unsigned char>(epsilon, delta) {}

    virtual void update(T item, std::size_t freq) override
    {
        for (auto i = 0UL; i < this->d; ++i)
        {
            auto & c = this->counters[i * this->w + this->hashes[i](item, this->w)];
            auto p = 1UL << c;
            for (auto j = 0U; j < freq; ++j)
                if (r(g) < 1.0 / p)
                {
                    ++c;
                    p <<= 1;
                }
        }
    }

    virtual unsigned long query(T item)
    {
        auto m = this->counters[this->hashes[0](item, this->w)];
        for (auto i = 1UL; i < this->d; ++i)
        {
            auto c = this->counters[i * this->w + this->hashes[i](item, this->w)];
            if (c < m)
                m = c;
        }
        return (1UL << m) - 1;
    }
private:
    static std::uniform_real_distribution<> r;
    static std::mt19937_64 g;
};

template<typename T>
std::uniform_real_distribution<> cms_morris<T>::r = std::uniform_real_distribution<>();

template<typename T>
std::mt19937_64 cms_morris<T>::g = std::mt19937_64();

int main()
{
    constexpr double epsilon = 0.01, delta = 0.01;
    cms_default<int> cms1(epsilon, delta);
    cms_conservative<int> cms2(epsilon, delta);
    cms_morris<int> cms3(epsilon, delta);

    std::cout << "Memory Footprint" << std::endl
              << "CMS Default: " << sizeof(cms1) + cms1.memory_allocated() << std::endl
              << "CMS Conservative: " << sizeof(cms2) + cms2.memory_allocated() << std::endl
              << "CMS Morris: " << sizeof(cms3) + cms3.memory_allocated() << std::endl;

    return 0;
}
