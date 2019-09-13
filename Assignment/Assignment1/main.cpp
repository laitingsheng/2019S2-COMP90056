#include <chrono>
#include <iomanip>
#include <iostream>
#include <unordered_map>

#include "cms.hpp"
#include "hash.hpp"
#include "utils.hpp"

template<typename Type, typename UpdateType>
struct stream
{
    using item_type = Type;
    using update_type = UpdateType;

    stream(UpdateType update_min, UpdateType update_max) : rf(update_min, update_max), g(std::random_device()()) {}

    // prevent unintentional copy
    stream(stream const &) = delete;

    virtual std::pair<Type, UpdateType> operator++(int) final
    {
        return { generate(), rf(g) };
    }
protected:
    std::mt19937_64 g;
private:
    std::uniform_int_distribution<UpdateType> rf;

    virtual Type generate() = 0;
};

template<typename IntType, typename UpdateType>
struct int_stream final : stream<IntType, UpdateType>
{
    int_stream(IntType int_min,
               IntType int_max,
               UpdateType update_min,
               UpdateType update_max) : ri(int_min, int_max), stream<IntType, UpdateType>(update_min, update_max) {}
private:
    std::uniform_int_distribution<IntType> ri;

    virtual IntType generate()
    {
        return ri(this->g);
    }
};

template<typename UpdateType>
struct string_stream final : stream<std::string, UpdateType>
{
    string_stream(std::size_t length_min,
                  std::size_t length_max,
                  UpdateType update_min,
                  UpdateType update_max) : rl(length_min, length_max),
                                           rc(32, 126),
                                           stream<std::string, UpdateType>(update_min, update_max){}
private:
    std::uniform_int_distribution<char> rc;
    std::uniform_int_distribution<std::size_t> rl;

    virtual std::string generate()
    {
        std::string re(rl(this->g), 0);
        for (char & c : re)
            c = rc(this->g);
        return std::move(re);
    }
};

template<typename StreamType>
class stream_runner final
{
    using item_type = typename StreamType::item_type;
    using update_type = typename StreamType::update_type;
    using counter_type = std::unordered_map<item_type, update_type>;

    double const epsilon, delta;

    template<typename CMS>
    void stat(CMS const & cms, counter_type const & counter)
    {
        std::cout << "CMS " << CMS::name() << ":" << std::endl
                  << "    Memory: " << sizeof(cms) + cms.memory_allocated() << " Bytes" << std::endl;

        std::size_t a = 0;
        double time = 0;
        for (auto & [k, v] : counter)
        {
            auto s = std::chrono::system_clock::now();
            update_type c = cms.query(k);
            auto e = std::chrono::system_clock::now();
            a += (c == v);
            time += std::chrono::duration<double>(e - s).count();
        }
        std::cout << std::fixed << std::setprecision(3)
                  << "    Accuracy: " << double(a) / counter.size() * 100 << "%" << std::endl
                  << "    Time: " << time * 1000 << " ms" << std::endl;
    }
public:
    stream_runner(double epsilon, double delta) : delta(delta), epsilon(epsilon) {}

    template<typename... CMSs>
    void run(std::size_t stream_size, StreamType && stream, CMSs &&... cmss)
    {
        counter_type counter;
        for (std::size_t i = 0; i < stream_size; ++i)
        {
            auto [item, update] = stream++;
            counter[item] += update;
            (cmss.update(item, update), ...);
        }

        // count bucket size for accurate memory usage
        constexpr auto pair_size = sizeof(std::pair<item_type, update_type>);
        std::size_t c = sizeof(counter);
        for (std::size_t i = 0; i < counter.bucket_count(); ++i)
        {
            auto s = counter.bucket_size(i);
            // empty bucket will have at least one empty slot
            c += (s == 0 ? 1 : s) * pair_size;
        }
        for (auto & [k, v] : counter)
            c += type_size<item_type>::runtime_size(k) + type_size<update_type>::runtime_size(v);
        std::cout << "Counter Memory Usage (Estimate): " << c << " Bytes" << std::endl;

        (stat(cmss, counter), ...);
    }
};

int main()
{
    constexpr std::size_t stream_size = 100;
    constexpr double epsilon = 0.01, delta = 0.01;
    stream_runner<int_stream<int, std::size_t>>(epsilon, delta).run(stream_size,
                                                                    int_stream<int, std::size_t>(-10000,
                                                                                                 10000,
                                                                                                 200,
                                                                                                 300),
                                                                    cms_default<int, std::size_t>(epsilon, delta),
                                                                    cms_conservative<int, std::size_t>(epsilon, delta),
                                                                    cms_morris<int, std::size_t>(epsilon, delta));
    stream_runner<string_stream<std::size_t>>(epsilon,
                                              delta).run(stream_size,
                                                         string_stream<std::size_t>(20, 30, 200, 300),
                                                         cms_default<std::string, std::size_t>(epsilon, delta),
                                                         cms_conservative<std::string, std::size_t>(epsilon, delta),
                                                         cms_morris<std::string, std::size_t>(epsilon, delta));
    stream_runner<int_stream<int, long>>(epsilon, delta).run(stream_size,
                                                             int_stream<int, long>(-10000, 10000, -300, 300),
                                                             cms_default<int, long>(epsilon, delta),
                                                             cms_conservative<int, long>(epsilon, delta));
    stream_runner<string_stream<long>>(epsilon,
                                       delta).run(stream_size,
                                                  string_stream<long>(20, 30, -300, 300),
                                                  cms_default<std::string, long>(epsilon, delta),
                                                  cms_conservative<std::string, long>(epsilon, delta));
    return 0;
}
