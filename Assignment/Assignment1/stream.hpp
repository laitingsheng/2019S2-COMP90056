#ifndef __STREAM_HPP__
#define __STREAM_HPP__

#include <random>

template<typename Type, typename UpdateType>
struct stream
{    // prevent unintentional copy
    stream(stream const &) = delete;

    virtual std::pair<Type, UpdateType> operator++(int) = 0;

    virtual bool operator!() const = 0;
protected:
    std::size_t const num_distinct, min_repeat, max_repeat;

    stream(std::size_t num_distinct,
           std::size_t min_repeat,
           std::size_t max_repeat) : max_repeat(max_repeat),
                                     min_repeat(min_repeat),
                                     num_distinct(num_distinct),
                                     counter() {}
};

template<typename Type, typename UpdateType>
class int_stream : stream
{
    static constexpr bool is_unsigned_update = std::is_unsigned_v<QueryType>;

    std::size_t const max_freq;
public:
    int_stream(std::size_t num_distinct,
               std::size_t min_repeat,
               std::size_t max_repeat) : max_repeat(max_repeat),
                                         min_repeat(min_repeat),
                                         num_distinct(num_distinct),
                                         counter() {}
};

#endif
