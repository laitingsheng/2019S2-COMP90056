#include <iostream>

#include "cms.hpp"
#include "hash.hpp"
#include "stream.hpp"

template<typename IntType, bool force_positive_update = true>
static void run_int_stream(double epsilon,
                           double delta,
                           uint32_t num_distinct,
                           uint8_t min_repeat,
                           uint8_t max_repeat,
                           std::conditional_t<force_positive_update, uint16_t, int16_t> min_update,
                           std::conditional_t<force_positive_update, uint16_t, int16_t> max_update,
                           IntType min_value = std::numeric_limits<IntType>::min(),
                           IntType max_value = std::numeric_limits<IntType>::max())
{
    using CounterType = std::conditional_t<force_positive_update, uint64_t, int64_t>;

    run_stream(int_stream<IntType, force_positive_update>(num_distinct,
                                                          min_repeat,
                                                          max_repeat,
                                                          min_update,
                                                          max_update,
                                                          min_value,
                                                          max_value),
               cms_default<IntType, CounterType>(epsilon, delta),
               cms_conservative<IntType, CounterType>(epsilon, delta),
               cms_morris<IntType, CounterType>(epsilon, delta));
}

int main()
{
    for (uint32_t i = 1000; i < 10000000U; i *= 10)
    {
        std::cout << "Number of distinct items in the stream: " << i << std::endl;
        run_int_stream<int64_t>(0.01, 0.01, i, 3, 5, 0, 500, 0, 2 * i);
        run_int_stream<int64_t, false>(0.01, 0.01, i, 3, 5, -500, 500, -int64_t(i), i);
        std::cout << std::endl;
    }
    return 0;
}
