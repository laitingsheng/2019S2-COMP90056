#ifndef __CMS_HPP__
#define __CMS_HPP__

#include "hash.hpp"
#include "morris.hpp"

template<typename Type, typename CounterType = std::size_t>
struct cms_template
{
    // prevent copy and move
    cms_template(cms_template const &) = delete;
    cms_template(cms_template &&) = delete;

    virtual std::size_t memory_allocated() const final
    {
        return sizeof(hash<Type>) * d + sizeof(CounterType) * d * w;
    }

    virtual ~cms_template()
    {
        delete[] hashes;
        delete[] counters;

        w = d = 0;
        hashes = nullptr;
        counters = nullptr;
    }

    virtual void update(Type item, std::size_t freq)
    {
        for (auto i = 0UL; i < this->d; ++i)
            this->counters[i * this->w + this->hashes[i](item, this->w)] += freq;
    }

    virtual std::size_t query(Type item) const
    {
        std::size_t m = this->counters[this->hashes[0](item, this->w)];
        for (auto i = 1UL; i < this->d; ++i)
        {
            std::size_t c = this->counters[i * this->w + this->hashes[i](item, this->w)];
            if (c < m)
                m = c;
        }
        return m;
    }
protected:
    std::size_t w, d;
    hash<Type> const * hashes;
    CounterType * counters;

    cms_template(double epsilon, double delta) : counters(new CounterType[w * d]()),
                                                 hashes(new hash<Type>[d]()),
                                                 d(ceil(log(1 / delta))),
                                                 w(ceil(2 / epsilon / epsilon)) {}
};

template<typename Type>
struct cms_default : public cms_template<Type>
{
    static inline constexpr std::string name()
    {
        return std::string("Default<") + typeid(Type).name() + ">";
    }

    explicit cms_default(double epsilon, double delta) : cms_template<Type>(epsilon, delta) {}
};

template<typename Type>
struct cms_conservative : public cms_default<Type>
{
    static inline constexpr std::string name()
    {
        return std::string("Conservative<") + typeid(Type).name() + ">";
    }

    explicit cms_conservative(double epsilon, double delta) : cms_default<Type>(epsilon, delta) {}

    virtual void update(Type item, std::size_t freq) override
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

template<typename Type>
struct cms_morris : public cms_template<Type, morris_counter>
{
    static inline constexpr std::string name()
    {
        return std::string("Morris<") + typeid(Type).name() + ">";
    }

    explicit cms_morris(double epsilon, double delta) : cms_template<Type, morris_counter>(epsilon, delta) {}
};

#endif
