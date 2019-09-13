#ifndef __CMS_HPP__
#define __CMS_HPP__

#include "hash.hpp"
#include "morris.hpp"
#include "utils.hpp"

template<typename Type, typename CounterType = std::size_t, typename QueryType = CounterType>
struct cms_template
{
    // prevent copy and move
    cms_template(cms_template const &) = delete;
    cms_template(cms_template &&) = delete;

    virtual std::size_t memory_allocated() const
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

    virtual void update(Type item, QueryType freq)
    {
        for (auto i = 0UL; i < this->d; ++i)
            this->counters[i * this->w + this->hashes[i](item, this->w)] += freq;
    }

    virtual QueryType query(Type item) const
    {
        QueryType m = this->counters[this->hashes[0](item, this->w)];
        for (auto i = 1UL; i < this->d; ++i)
        {
            QueryType c = this->counters[i * this->w + this->hashes[i](item, this->w)];
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
                                                 d(ceil(log2(1 / delta))),
                                                 w(ceil(2 / epsilon)) {}
};

template<typename Type, typename CounterType = std::size_t>
struct cms_default final : public cms_template<Type, CounterType>
{
    static inline constexpr std::string name()
    {
        return std::string("Default<") + type_name<Type>::name + ">";
    }

    explicit cms_default(double epsilon, double delta) : cms_template<Type, CounterType>(epsilon, delta) {}
};

template<typename Type, typename CounterType = std::size_t>
struct cms_conservative final : public cms_template<Type, CounterType>
{
    static inline constexpr std::string name()
    {
        return std::string("Conservative<") + type_name<Type>::name + ">";
    }

    explicit cms_conservative(double epsilon, double delta) : cms_template<Type, CounterType>(epsilon, delta) {}

    virtual void update(Type item, CounterType freq) override
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

template<typename Type, typename CounterType, bool=std::is_unsigned_v<CounterType>>
struct cms_morris;

template<typename Type, typename CounterType>
struct cms_morris<Type, CounterType, true> final : public cms_template<Type, morris_counter, CounterType>
{
    static inline constexpr std::string name()
    {
        return std::string("Morris<") + type_name<Type>::name + ">";
    }

    explicit cms_morris(double epsilon, double delta) : cms_template<Type, morris_counter, CounterType>(epsilon, delta)
    {}
};

template<typename Type, typename CounterType>
struct cms_morris<Type, CounterType, false> final : public cms_template<Type, morris_counter, CounterType>
{
    static inline constexpr std::string name()
    {
        return std::string("Morris<") + type_name<Type>::name + ">";
    }

    explicit cms_morris(double epsilon, double delta) : cms_template<Type, morris_counter, CounterType>(epsilon, delta)
    {}
};

#endif
