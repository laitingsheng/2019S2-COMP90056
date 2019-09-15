#include "cms.hpp"
#include "hash.hpp"
#include "stream.hpp"

template<typename IntType, bool force_positive_update = true>
static void run_int_stream(double epsilon,
                           double delta,
                           uint16_t num_distinct,
                           uint8_t min_repeat,
                           uint8_t max_repeat,
                           std::conditional_t<force_positive_update, uint16_t, int16_t> min_update,
                           std::conditional_t<force_positive_update, uint16_t, int16_t> max_update,
                           IntType min_value = std::numeric_limits<IntType>::min(),
                           IntType max_value = std::numeric_limits<IntType>::max())
{
    using CounterType = std::conditional_t<force_positive_update, uint64_t, int64_t>;

    run_stream(epsilon,
               int_stream<IntType, force_positive_update>(num_distinct,
                                                          min_repeat,
                                                          max_repeat,
                                                          min_update,
                                                          max_update,
                                                          min_value,
                                                          max_value),
               cms_default<IntType, CounterType>(epsilon, delta),
               cms_conservative<IntType, CounterType>(epsilon, delta));
}

int main()
{
    run_int_stream<int64_t>(0.01, 0.01, 1000, 3, 5, 0, 500);
    return 0;
}
