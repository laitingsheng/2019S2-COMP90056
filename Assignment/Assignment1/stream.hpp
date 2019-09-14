#ifndef __STREAM_HPP__
#define __STREAM_HPP__

#include <cstdint>
#include <random>

template<typename Type, bool force_positive_update = true>
struct stream
{
    using UpdateType = std::conditional_t<force_positive_update, uint16_t, int16_t>;
    using Counter = std::unordered_map<Type, int32_t>;

    std::uniform_int_distribution<uint8_t> rb;

    uint8_t const min_repeat, max_repeat;
    std::uniform_int_distribution<uint16_t> rs;
    std::uniform_int_distribution<UpdateType> rf;

    std::unordered_map<Type, std::pair<int32_t, uint16_t>> record;
    std::vector<Type> distinct;
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
                                    rs(0, num_distinct - 1),
                                    max_repeat(max_repeat),
                                    min_repeat(min_repeat),
                                    distinct(num_distinct),
                                    record(num_distinct)
    {
        size_t i = 0;
        while (i < num_distinct)
        {
            auto item = generate();
            if (!record.count(item))
            {
                distinct[i++] = item;
                record[std::move(item)] = {};
            }
        }
    }
public:
    // prevent unintentional copy
    stream(stream const &) = delete;

    virtual Counter counter() const final
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
            Type const & item = distinct[rs(g)];
            auto & [f, r] = record[item];
            if (r == max_repeat)
                continue;
            int32_t freq;
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
            F1 += frq;
            ++r;
            return { item, freq }
        }
        throw "exceeding stream limit";
    }

    virtual bool operator!() const final
    {
        for (auto const & [_, v] : record)
        {
            auto const & [f, r] = v;
            if (r < min_repeat || f < 0)
                return false;
        }
        return true;
    }

    virtual operator int64_t() const final
    {
        return F1;
    }
};

template<typename IntType, bool force_positive_update = true>
class int_stream : public stream<IntType, force_positive_update>
{
    std::uniform_int_distribution<IntType> ri;

    virtual IntType generate() override
    {
        return ri(this->g);
    }
public:
    int_stream(uint16_t num_distinct,
               uint8_t min_repeat,
               uint8_t max_repeat,
               UpdateType min_update,
               UpdateType max_update,
               IntType min_value,
               IntType max_value) : ri(min_value, max_value),
                                    stream(num_distinct, min_repeat, max_repeat, min_update, max_update) {}
}

#endif
