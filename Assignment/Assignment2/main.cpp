#include <cinttypes>
#include <random>

#include "stream.hpp"

int main()
{
    insertion_stream<std::int64_t, std::uniform_int_distribution<std::uint8_t>> uniform_insertion_stream(-1000, 1000);
    return 0;
}
