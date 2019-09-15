#ifndef __CMS_HPP__
#define __CMS_HPP__

#include <cstdint>

#include "hash.hpp"
#include "morris.hpp"
#include "utils.hpp"

template<typename Type, typename CounterType = uint64_t, typename QueryType = CounterType>
struct cms_template
{
    double const epsilon, delta;

    // prevent copy and move
    cms_template(cms_template const &) = delete;
    cms_template(cms_template &&) = delete;

    virtual uint64_t memory_allocated() const
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
        if (!freq)
            return;

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
    uint64_t w, d;
    hash<Type> const * hashes;
    CounterType * counters;

    cms_template(double epsilon, double delta) : counters(new CounterType[w * d]()),
                                                 hashes(new hash<Type>[d]()),
                                                 d(ceil(log2(1 / delta))),
                                                 w(ceil(2 / epsilon)),
                                                 delta(delta),
                                                 epsilon(epsilon) {}
};

template<typename Type, typename CounterType = uint64_t>
struct cms_default final : public cms_template<Type, CounterType>
{
    static inline constexpr std::string name()
    {
        return std::string("Default<") + type_name<Type>::name + ">";
    }

    explicit cms_default(double epsilon, double delta) : cms_template<Type, CounterType>(epsilon, delta) {}
};

template<typename Type, typename CounterType = uint64_t>
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

template<typename Type, typename QueryType, bool = std::is_unsigned_v<QueryType>>
struct cms_morris;

template<typename Type, typename QueryType>
struct cms_morris<Type, QueryType, true> final : public cms_template<Type, morris_counter, QueryType>
{
    static inline constexpr std::string name()
    {
        return std::string("Morris<") + type_name<Type>::name + ">";
    }

    explicit cms_morris(double epsilon, double delta) : cms_template<Type, morris_counter, QueryType>(epsilon, delta) {}
};

template<typename Type, typename QueryType>
struct cms_morris<Type, QueryType, false> final : public cms_template<Type, morris_counter, QueryType>
{
    static inline constexpr std::string name()
    {
        return std::string("Morris<") + type_name<Type>::name + ">";
    }

    virtual std::size_t memory_allocated() const
    {
        return sizeof(hash<Type>) * this->d + sizeof(morris_counter) * this->d * this->w * 2;
    }

    explicit cms_morris(double epsilon, double delta) : counters_neg(new morris_counter[this->w * this->d]()),
                                                        cms_template<Type, morris_counter, QueryType>(epsilon, delta) {}

    virtual ~cms_morris()
    {
        delete[] counters_neg;
        counters_neg = nullptr;
    }

    virtual void update(Type item, QueryType freq) override
    {
        if (!freq)
            return;
        if (freq < 0)
        {
            freq = -freq;
            for (auto i = 0UL; i < this->d; ++i)
                counters_neg[i * this->w + this->hashes[i](item, this->w)] += freq;
        }
        else
            for (auto i = 0UL; i < this->d; ++i)
                this->counters[i * this->w + this->hashes[i](item, this->w)] += freq;
    }

    virtual QueryType query(Type item) const override
    {
        auto h = this->hashes[0](item, this->w);
        QueryType p = this->counters[h], n = counters_neg[h], m = p - n;
        for (auto i = 1UL; i < this->d; ++i)
        {
            h = i * this->w + this->hashes[i](item, this->w);
            p = this->counters[h];
            n = counters_neg[h];
            auto c = p - n;
            if (c < m)
                m = c;
        }
        return m;
    }
private:
    morris_counter * counters_neg;
};

#endif
