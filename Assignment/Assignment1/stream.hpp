#ifndef __STREAM_HPP__
#define __STREAM_HPP__

#include <cstdint>
#include <random>

template<typename Type, bool force_positive_update = true>
struct stream
{
    using UpdateType = std::conditional_t<force_positive_update, uint16_t, int16_t>;
    using Counter = std::unordered_map<Type, int32_t>;

    static std::uniform_int_distribution<uint8_t> rb;

    uint8_t const min_repeat, max_repeat;
    std::uniform_int_distribution<uint16_t> rs;
    std::uniform_int_distribution<UpdateType> rf;

    std::unordered_map<Type, std::pair<int32_t, uint8_t>> record;
    int64_t F1;

    virtual Type generate() = 0;
protected:
    std::mt19937_64 g;

    stream(uint16_t num_distinct,
           uint8_t min_repeat,
           uint8_t max_repeat,
           UpdateType min_update,
           UpdateType max_update) : g(std::random_device()()),
                                    rf(min_update, max_update),
                                    rs(0, num_distinct),
                                    max_repeat(max_repeat),
                                    min_repeat(min_repeat),
                                    record(num_distinct)
    {
        while (record.size() < num_distinct)
        {
            auto item = generate();
            if (!record.count(item))
                record[std::move(item)] = {};
        }
    }
public:
    // prevent unintentional copy
    stream(stream const &) = delete;

    virtual Counter counter() final
    {
        Counter counter;
        for (auto const & [k, v] : record)
            counter[k] = v.first();
        return counter;
    }

    virtual std::pair<Type, UpdateType> operator++(int) final
    {
        while (true)
        {
            Type item = generate();
            auto & r = repeats[item];
            if (r < max_repeat)
            {
                ++r;
                if constexpr (force_positive_update)
                {
                    int32_t f = rf(g);
                    counter[item] += f;
                    F1 += f;
                    return { item, f };
                }
                else
                {}
            }
        }
        throw "exceeding stream limit";
    }

    virtual bool operator!() const final
    {
        for (auto const & [_, v] : repeats)
            if (v < min_repeat)
                return false;
        return counter.size() == num_distinct;
    }

    virtual operator int64_t() const final
    {
        return F1;
    }
};

#endif
