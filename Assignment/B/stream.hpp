#ifndef __STREAM_HPP__
#define __STREAM_HPP__

#include <algorithm>
#include <cstdint>
#include <random>
#include <stdexcept>
#include <vector>

namespace stream
{

template<typename T, typename U>
struct turnstile_stream;

template<>
struct turnstile_stream<uint16_t, int8_t> final
{
    turnstile_stream(uint64_t pn, uint64_t nn) : ud(1), id(1), g(rd()), nn(nn), pn(pn)
    {
        if (nn > pn)
            throw std::invalid_argument("invalid turnstile stream");
        generated.reserve(nn);
    }

    operator std::pair<bool, std::pair<uint16_t, int8_t>>()
    {
        if (nn && generated.size() && (!pn || coin(g)))
        {
            std::random_shuffle(generated.begin(), generated.end());
            std::pair<uint16_t, int8_t> re = std::move(generated.back());
            generated.pop_back();
            --nn;
            return { true, std::move(re) };
        }
        if (!pn)
            return { false, {} };
        uint16_t item = id(g);
        int8_t update = ud(g);
        generated.emplace_back(item, -update);
        --pn;
        return { true, { item, update } };
    }

    operator std::vector<std::pair<uint16_t, int8_t>>() const noexcept
    {
        return generated;
    }
private:
    friend struct turnstile_stream_tester;

    static std::random_device rd;

    uint64_t pn, nn;

    std::mt19937 g;
    std::bernoulli_distribution coin;
    std::uniform_int_distribution<uint16_t> id;
    std::uniform_int_distribution<int8_t> ud;

    std::vector<std::pair<uint16_t, int8_t>> generated;
};

std::random_device turnstile_stream<uint16_t, int8_t>::rd {};

}

#endif
