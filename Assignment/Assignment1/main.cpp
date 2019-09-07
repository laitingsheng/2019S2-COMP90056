#include <array>
#include <iomanip>
#include <iostream>
#include <random>
#include <unordered_map>
#include <unordered_set>
#include <utility>

#include <omp.h>

#include "cms.hpp"
#include "hash.hpp"

template<typename T, std::size_t stream_size>
struct stream_generator final
{
    std::tuple<std::array<std::pair<T, std::size_t>, stream_size>,
               std::unordered_map<T, std::size_t>,
               std::size_t> operator()(std::size_t freq_min, std::size_t freq_max)
    {
        // only applicable to integer type
        auto ri = std::uniform_int_distribution<T>(std::numeric_limits<T>::min());
        auto rf = std::uniform_int_distribution<std::size_t>(freq_min, freq_max);
        auto g = std::random_device();

        std::array<std::pair<T, std::size_t>, stream_size> stream;
        for (auto & i : stream)
            i = { ri(g), rf(g) };

        std::unordered_map<T, std::size_t> counter;
        for (auto const & [k, v] : stream)
            counter[k] += v;

        size_t c = 0;
        for (size_t i = 0; i < counter.bucket_count(); ++i)
        {
            auto s = counter.bucket_size(i);
            if (s == 0)
                ++c;
            else
                c += s;
        }

        return { stream, counter, c * sizeof(std::pair<T, std::size_t>) };
    }
};

template<std::size_t stream_size>
struct stream_generator<std::string, stream_size> final
{
    std::tuple<std::array<std::pair<std::string, std::size_t>, stream_size>,
               std::unordered_map<std::string, std::size_t>,
               std::size_t> operator()(std::size_t freq_min, std::size_t freq_max)
    {
        // all ASCII printable characters
        auto ri = std::uniform_int_distribution<char>(32, 126);
        auto rf = std::uniform_int_distribution<std::size_t>(freq_min, freq_max);
        auto g = std::random_device();

        std::unordered_set<std::string> strings;

        std::array<std::pair<std::string, std::size_t>, stream_size> stream;
        for (auto & i : stream)
        {
            std::string str(rf(g), 0);
            for (char & c : str)
                c = ri(g);
            i = { str, rf(g) };
            strings.insert(std::move(str));
        }

        std::unordered_map<std::string, std::size_t> counter;
        for (auto const & [k, v] : stream)
            counter[k] += v;

        size_t c = 0;
        for (size_t i = 0; i < counter.bucket_count(); ++i)
        {
            auto s = counter.bucket_size(i);
            if (s == 0)
                ++c;
            else
                c += s;
        }
        c *= sizeof(std::pair<std::string, std::size_t>);
        for (auto const & s : strings)
            c += s.size();

        return { stream, counter, c };
    }
};

template<typename T, std::size_t stream_size, std::size_t run_num, typename... CMSs>
struct cms_test_stat final
{
    cms_test_stat(double epsilon, double delta) : delta(delta), epsilon(epsilon) {}

    void run(size_t freq_min, size_t freq_max)
    {
        auto [stream, counter, size] = stream_generator<T, stream_size>()(freq_min, freq_max);
        std::cout << "Counter Memory Usage (Estimate): " << size << " Bytes" << std::endl;

        (single_test<CMSs>(stream, counter), ...);
    }
private:
    double const epsilon, delta;

    template<typename CMS>
    inline void single_test(std::array<std::pair<T, std::size_t>, stream_size> const & stream,
                            std::unordered_map<T, std::size_t> const & counter) const
    {
        std::cout << "CMS " << CMS::name() << ":" << std::endl
                  << "    memory: " << CMS(epsilon, delta).memory_allocated() << " Bytes" << std::endl;

        double accuracy = 0, time = 0, lb = 1 - epsilon, ub = 1 + epsilon;
        #pragma omp parallel for reduction(+:accuracy, time)
        for (std::size_t i = 0; i < run_num; ++i)
        {
            CMS cms(epsilon, delta);
            for (auto & [k, v] : stream)
                cms.update(k, v);
            std::size_t a = 0;
            for (auto & [k, v] : counter)
            {
                double s = omp_get_wtime();
                size_t c = cms.query(k);
                double e = omp_get_wtime();
                time += e - s;
                if (c >= v * lb && c <= v * ub)
                    ++a;
            }
            accuracy += double(a) / counter.size();
        }

        std::cout << std::fixed << std::setprecision(3)
                  << "    accuracy: " << accuracy / run_num * 100 << "%" << std::endl
                  << "    time: " << time / run_num * 1000 << " ms" << std::endl;
    }
};

template<typename T, std::size_t stream_size, std::size_t run_num, std::size_t freq_min, std::size_t freq_max>
void test(double epsilon, double delta)
{
    cms_test_stat<T,
                  stream_size,
                  run_num,
                  cms_default<T>,
                  cms_conservative<T>,
                  cms_morris<T>>(epsilon, delta).run(freq_min, freq_max);
}

int main()
{
    test<int, 10000, 80, 100, 200>(0.01, 0.01);
    test<std::string, 10000, 80, 100, 200>(0.01, 0.01);
    test<std::string, 10000, 80, 200, 300>(0.01, 0.01);
    return 0;
}
