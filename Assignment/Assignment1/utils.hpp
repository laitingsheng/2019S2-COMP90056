#ifndef __UTILS_HPP__
#define __UTILS_HPP__

#include <string>

template<typename>
struct type_name;

template<>
struct type_name<long> final
{
    type_name() = delete;

    static inline constexpr auto name = "long";
};

template<>
struct type_name<unsigned long> final
{
    type_name() = delete;

    static inline constexpr auto name = "unsigned_long";
};

#endif
