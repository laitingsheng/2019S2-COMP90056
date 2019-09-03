#include <random>

template<typename T>
struct hash final
{
    hash()
    {
        auto r = std::uniform_int_distribution<unsigned int>(1, p);
        auto g = std::mt19937();
        a = r(g);
        b = r(g) - 1;
    }

    // prevent unintentional copy
    hash(hash const &) = delete;
    hash & operator=(hash const &) = delete;

    // force cast back to std::size_t as defined in the STL
    std::size_t operator()(T item, std::size_t domain) const
    {
        return (static_cast<unsigned long>(a) * basic(item) + b) % p % domain;
    }
private:
    static constexpr unsigned int p = 1073741789;
    static std::hash<T> const basic;

    unsigned int const a, b;
};

template<typename T>
struct cms_default
{
    explicit cms_default(double epsilon, double delta)
    {
        w = ceil(2 / epsilon / epsilon);
        d = ceil(log(1 / delta));
        hashes = new hash[d];
        init();
    }

    virtual void update(T item, std::size_t freq)
    {
        for (int i = 0; i < d; ++i)
            C[i * w + hashes[i](item)] += freq;
    }

    virtual unsigned long query(T item)
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
};

template<typename T>
struct cms_conservative final : public cms_default<T>
{
    explicit cms_conservative(double epsilon, double delta) : cms_default<T>(epsilon, delta) {}

    virtual void update(T item, std::size_t freq) override
    {
        auto f = query(item) + freq;
        for (int i = 0; i < this->d; ++i)
        {
            auto & c = this->C[i * this->w + this->hashes[i](item)];
            if (f > c)
                c = f;
        }
    }
};

template<typename T>
struct cms_morris final : public cms_default<T>
{
    explicit cms_morris(double epsilon, double delta) : cms_default<T>(epsilon, delta) {}

    virtual void update(T item, std::size_t freq) override
    {
        for (int i = 0; i < this->d; ++i)
        {
            auto & p = this->C[i * this->w + this->hashes[i](item)];
            auto r = std::uniform_real_distribution<>();
            auto g = std::mt19937_64();
            for (int j = 0; j < freq; ++j)
                if (r(g) < 1.0 / p)
                    p <<= 1;
        }
    }

    virtual unsigned long query(T item)
    {
        return cms_default<T>::query(item) - 1;
    }
protected:
    virtual inline void init() override
    {
        auto t = this->d * this->w;
        this->C = new unsigned long[t];
        for (int i = 0; i < t; ++i)
            this->C[i] = 1;
    }
};

int main()
{
    return 0;
}
