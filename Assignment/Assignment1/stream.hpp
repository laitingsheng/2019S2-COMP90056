#ifndef __STREAM_HPP__
#define __STREAM_HPP__

#include <cstdint>

#include <random>
#include <unordered_map>

template<typename Type, bool force_positive_update = true>
class stream
{
    using Counter = std::unordered_map<Type, int32_t>;
    using UpdateType = std::conditional_t<force_positive_update, uint16_t, int16_t>;

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
        uint16_t i = 0;
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

    virtual operator Counter() const final
    {
        Counter counter;
        for (auto const & [k, v] : record)
            counter[k] = v.first();
        return std::move(counter);
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
            F1 += freq;
            ++r;
            return { item, freq };
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
    using UpdateType = std::conditional_t<force_positive_update, uint16_t, int16_t>;

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
                                    stream<IntType, force_positive_update>(num_distinct,
                                                                           min_repeat,
                                                                           max_repeat,
                                                                           min_update,
                                                                           max_update) {}
};

template<typename CMS>
static void stat_single(CMS const & cms, Counter const & counter, double bound)
{
    std::cout << "CMS " << CMS::name() << ":" << std::endl
              << "    Memory: " << sizeof(cms) + cms.memory_allocated() << " Bytes" << std::endl;

    uint16_t a = 0;
    double time = 0;
    for (auto const & [k, v] : counter)
    {
        auto s = std::chrono::system_clock::now();
        auto c = cms.query(k);
        auto e = std::chrono::system_clock::now();
        a += (c < v + bound);
        time += std::chrono::duration<double>(e - s).count();
    }
    std::cout << std::fixed << std::setprecision(3)
                << "    Accuracy: " << double(a) / counter.size() * 100 << "%" << std::endl
                << "    Time: " << time * 1000 << " ms" << std::endl;

}

template<typename StreamType, typename... CMSs>
void run_stream(double epsilon, StreamType && stream, CMSs &&... cmss)
{
    while (!stream)
    {
        auto [k, v] = stream++;
        (cmss.update(k, v), ...);
    }

    Counter counter = stream;
    int64_t F1 = stream;

    // count bucket size for accurate memory usage
    constexpr auto pair_size = sizeof(std::pair<item_type, update_type>);
    size_t c = sizeof(counter);
    for (size_t i = 0; i < counter.bucket_count(); ++i)
    {
        auto s = counter.bucket_size(i);
        // empty bucket will have at least one empty slot
        c += (s == 0 ? 1 : s) * pair_size;
    }
    for (auto & [k, v] : counter)
        c += type_size<item_type>::runtime_size(k) + type_size<update_type>::runtime_size(v);
    std::cout << "Counter Memory Usage (Estimate): " << c << " Bytes" << std::endl;

    auto bound = epsilon * F1;
    (stat_single(cmss, counter, bound), ...);
}

#endif
