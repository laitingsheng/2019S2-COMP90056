#include <array>
#include <iostream>
#include <random>
#include <unordered_map>

#include <omp.h>

template<typename T>
struct hash final
{
    hash() : b(r(g) - 1), a(r(g)) {}

    // prevent unintentional copy
    hash(hash const &) = delete;
    hash & operator=(hash const &) = delete;

    // force cast back to std::size_t as defined in the STL
    inline std::size_t operator()(T item, unsigned long domain) const
    {
        // a is cast to unsigned long beforehand to avoid integer overflow
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

    virtual std::size_t memory_allocated() const final
    {
        return sizeof(hash<T>) * d + sizeof(C) * d * w;
    }

    virtual ~cms()
    {
        delete[] hashes;
        delete[] counters;
    }

    virtual void update(T item, std::size_t freq) = 0;
    virtual unsigned long query(T item) const = 0;
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

    virtual unsigned long query(T item) const
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
            for (std::size_t j = 0; j < freq; ++j)
                if (r(g) < 1.0 / p)
                {
                    ++c;
                    p <<= 1;
                }
        }
    }

    virtual unsigned long query(T item) const
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

struct stat final
{
    double const accuracy, time;

    stat(double accuracy, double time) : time(time), accuracy(accuracy) {}

    stat(stat const &) = default;
    stat & operator=(stat const &) = default;

    stat operator+(stat const & o) const
    {
        return stat(accuracy + o.accuracy, time + o.time);
    }

    stat operator/(std::size_t num) const
    {
        return stat(accuracy / num, time / num);
    }
};

template<typename T,
         std::size_t stream_size,
         std::size_t run_num,
         typename... CMSs>
struct cms_test_stat final
{
    cms_test_stat(double epsilon, double delta) : delta(delta), epsilon(epsilon) {}

    void run() {
    }
private:
    double const epsilon, delta;

    template<typename CMS>
    inline stat single_test(std::array<T, stream_size> const & items,
                            std::array<std::size_t, stream_size> const & freqs,
                            std::unordered_map<int, std::size_t> const & counter) const
    {
        CMS cms(epsilon, delta);
        std::cout << "CMS Default:" << std::endl
                  << "    memory: " << cms.memory_allocated() << std::endl;

        for (std::size_t i = 0; i < stream_size; ++i)
            cms.update(items[i], freqs[i]);

        return stat(0, 0, 0);
    }
};

int main()
{
    constexpr double epsilon = 0.01, delta = 0.01;
    cms_default<int> cms1(epsilon, delta);
    cms_conservative<int> cms2(epsilon, delta);
    cms_morris<int> cms3(epsilon, delta);

    cms_test_stat<int, 10000, 100, cms_default<int>>(0.01, 0.01).run();

    return 0;
}
