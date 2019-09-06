#include <array>
#include <iostream>
#include <random>
#include <unordered_map>
#include <utility>

#include <omp.h>

#include "cms.hpp"
#include "hash.hpp"

template<typename T, std::size_t stream_size, std::size_t run_num, typename... CMSs>
struct cms_test_stat final
{
    cms_test_stat(double epsilon, double delta) : delta(delta), epsilon(epsilon) {}

    void run() {
        auto ri = std::uniform_int_distribution<T>(std::numeric_limits<T>::min()),
             rf = std::uniform_int_distribution<std::size_t>(0, 100);
        auto g = std::random_device();

        auto items = std::array<T, stream_size>(), freqs = std::array<std::size_t, stream_size>();
        for (std::size_t i = 0; i < stream_size; ++i)
        {
            items[i] = ri(g);
            freqs[i] = rf(g);
        }

        auto counter = std::unordered_map<T, std::size_t>();
        for (std::size_t i = 0; i < stream_size; ++i)
            counter[items[i]] += freqs[i];

        size_t c = 0;
        for (size_t i = 0; i < counter.bucket_count(); ++i)
        {
            auto s = counter.bucket_size(i);
            if (s == 0)
                ++c;
            else
                c += s;
        }
        std::cout << "Counter Memory Usage: " << c * sizeof(std::pair<T, std::size_t>) << std::endl;

        (single_test<CMSs>(items, freqs, counter), ...);
    }
private:
    double const epsilon, delta;

    template<typename CMS>
    inline void single_test(std::array<T, stream_size> const & items,
                            std::array<std::size_t, stream_size> const & freqs,
                            std::unordered_map<int, std::size_t> const & counter) const
    {
        std::cout << "CMS " << CMS::name() << ":" << std::endl
                  << "    memory: " << CMS(epsilon, delta).memory_allocated() << std::endl;

        double accuracy = 0, time = 0, lb = 1 - epsilon, ub = 1 + epsilon;
        #pragma omp parallel for reduction(+:accuracy, time)
        for (std::size_t i = 0; i < run_num; ++i)
        {
            CMS cms(epsilon, delta);
            for (std::size_t i = 0; i < stream_size; ++i)
                cms.update(items[i], freqs[i]);
            std::size_t a = 0;
            for (auto & [k, v] : counter)
            {
                size_t c;
                double s = omp_get_wtime();
                c = cms.query(k);
                double e = omp_get_wtime();
                time += e - s;
                if (c >= v * lb && c <= v * ub)
                    ++a;
            }
            accuracy += double(a) / counter.size();
        }

        std::cout << "    accuracy: " << accuracy / run_num << std::endl
                  << "    time: " << time / run_num << std::endl;
    }
};

int main()
{
    constexpr double epsilon = 0.01, delta = 0.01;
    cms_default<int> cms1(epsilon, delta);
    cms_conservative<int> cms2(epsilon, delta);
    cms_morris<int> cms3(epsilon, delta);

    cms_test_stat<int, 10000, 80, cms_default<int>, cms_conservative<int>, cms_morris<int>>(0.01, 0.01).run();

    return 0;
}
