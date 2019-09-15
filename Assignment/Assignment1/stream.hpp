#ifndef __STREAM_HPP__
#define __STREAM_HPP__

#include <cassert>
#include <cstdint>

#include <chrono>
#include <iomanip>
#include <iostream>
#include <random>
#include <unordered_map>

#include "utils.hpp"

template<typename Type, bool force_positive_update = true>
class stream
{
    using Counter = std::unordered_map<Type, std::conditional_t<force_positive_update, uint64_t, int64_t>>;
    using UpdateType = std::conditional_t<force_positive_update, uint16_t, int16_t>;

    std::uniform_int_distribution<uint8_t> rb;

    uint8_t const min_repeat, max_repeat;
    std::uniform_int_distribution<uint32_t> rs;
    std::uniform_int_distribution<UpdateType> rf;

    virtual Type generate() = 0;
protected:
    std::unordered_map<Type, std::pair<int64_t, uint8_t>> record;
    std::vector<Type> distinct;
    int64_t F1;

    std::mt19937_64 g;

    stream(uint32_t num_distinct,
           uint8_t min_repeat,
           uint8_t max_repeat,
           UpdateType min_update,
           UpdateType max_update) : g(std::random_device()()),
                                    rf(min_update, max_update),
                                    rs(0, num_distinct - 1),
                                    max_repeat(max_repeat),
                                    min_repeat(min_repeat),
                                    distinct(num_distinct),
                                    record(num_distinct) {}
public:
    // prevent unintentional copy
    stream(stream const &) = delete;

    virtual std::pair<Type, UpdateType> operator++(int) final
    {
        while (true)
        {
            Type const & item = distinct[rs(g)];
            auto & [f, r] = record[item];
            if (r == max_repeat)
                continue;
            int64_t freq;
            if constexpr (force_positive_update)
                freq = rf(g);
            else
            {
                if (r == max_repeat - 1)
                    if (f < 0)
                        freq = -f + rb(g);
                    else
                        freq = rb(g);
                else
                    freq = rf(g);
            }
            f += freq;
            F1 += freq;
            ++r;
            return { item, freq };
        }
    }

    virtual bool operator!() const final
    {
        for (auto const & [_, v] : record)
        {
            auto const & [f, r] = v;
            if (r < min_repeat || f < 0)
                return true;
        }
        return false;
    }
};

template<typename IntType, bool force_positive_update = true>
class int_stream final : public stream<IntType, force_positive_update>
{
    using UpdateType = std::conditional_t<force_positive_update, uint16_t, int16_t>;

    template<typename StreamType, typename... CMSs>
    friend void run_stream(StreamType && stream, CMSs &&... cmss);

    std::uniform_int_distribution<IntType> ri;

    virtual IntType generate() override
    {
        return ri(this->g);
    }

    void stat()
    {
        constexpr auto pair_size = sizeof(std::pair<IntType, int32_t>);
        size_t c = sizeof(this->record);
        for (size_t i = 0; i < this->record.bucket_count(); ++i)
        {
            auto s = this->record.bucket_size(i);
            // empty bucket will have at least one empty slot
            c += (s == 0 ? 1 : s) * pair_size;
        }
        std::cout << c << std::endl;
    }

    template<typename CMS>
    void measure(CMS & cms)
    {
        std::cout << sizeof(cms) + cms.memory_allocated() << " ";

        std::make_unsigned_t<IntType> a = 0;
        double min_diff = std::numeric_limits<double>::max(),
               max_diff = std::numeric_limits<double>::min(),
               sum_diff = 0;
        double time = 0;
        for (auto k = ri.min(); k <= ri.max(); ++k)
        {
            auto s = std::chrono::system_clock::now();
            auto c = cms.query(k);
            auto e = std::chrono::system_clock::now();
            double diff = (c - double(this->record.count(k) ? this->record[k].first : 0)) / this->F1;
            sum_diff += diff;
            if (diff < min_diff)
                min_diff = diff;
            if (diff > max_diff)
                max_diff = diff;
            a += diff <= cms.epsilon;
            time += std::chrono::duration<double>(e - s).count();
        }

        auto num = ri.max() - ri.min() + 1;
        double accuracy = double(a) / num;
        std::cout << std::setprecision(10) << std::boolalpha
                  << sum_diff / num << " "
                  << min_diff << " "
                  << max_diff << " "
                  << time * 1000 << " "
                  << accuracy << " "
                  << (accuracy >= 1 - cms.delta) << std::endl;
    }
public:
    int_stream(uint32_t num_distinct,
               uint8_t min_repeat,
               uint8_t max_repeat,
               UpdateType min_update,
               UpdateType max_update,
               IntType min_value,
               IntType max_value) : ri(min_value, max_value),
                                    stream<IntType, force_positive_update>(num_distinct,
                                                                           min_repeat,
                                                                           max_repeat,
                                                                           min_update,
                                                                           max_update)
    {
        uint32_t i = 0;
        while (i < num_distinct)
        {
            auto item = generate();
            if (!this->record.count(item))
            {
                this->distinct[i++] = item;
                this->record[std::move(item)] = {};
            }
        }
    }
};

template<typename CMS, typename Type, typename UpdateType>
static double time_update(CMS && cms, Type k, UpdateType v)
{
    auto s = std::chrono::system_clock::now();
    cms.update(k, v);
    auto e = std::chrono::system_clock::now();
    return std::chrono::duration<double>(e - s).count();
}

template<typename StreamType, typename... CMSs>
void run_stream(StreamType && stream, CMSs &&... cmss)
{
    constexpr auto num = sizeof...(cmss);
    double times[num] {};
    while (!stream)
    {
        auto [k, v] = stream++;
        double tmps[] { time_update(cmss, k, v)... };
        for (size_t i = 0; i < num; ++i)
            times[i] += tmps[i];
    }

    stream.stat();

    for (auto t : times)
        std::cout << std::fixed << std::setprecision(3) << t * 1000 << std::endl;

    (stream.measure(cmss), ...);
}

#endif
