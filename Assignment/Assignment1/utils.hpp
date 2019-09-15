#ifndef __UTILS_HPP__
#define __UTILS_HPP__

#include <string>

template<typename>
struct type_name;

template<>
struct type_name<std::string> final
{
    type_name() = delete;

    static inline constexpr auto name = "std::string";
};

template<>
struct type_name<long> final
{
    type_name() = delete;

    static inline constexpr auto name = "long";
};

template<typename T>
struct type_size final
{
    type_size() = delete;

    static inline constexpr std::size_t runtime_size(T const &)
    {
        return 0;
    }
};

template<>
struct type_size<std::string> final
{
    type_size() = delete;

    static inline std::size_t runtime_size(std::string const & s)
    {
        return s.size();
    }
};

#endif
