#ifndef __STREAM_HPP__
#define __STREAM_HPP__

#include <algorithm>
#include <cstdint>
#include <limits>
#include <random>
#include <stdexcept>
#include <type_traits>
#include <unordered_set>
#include <vector>

namespace stream
{

enum class stream_type
{
    turnstile,
    general
};

template<stream_type ST>
struct int_sparse_stream final
{
    int_sparse_stream(uint64_t, uint64_t);

    operator std::pair<bool, std::pair<uint16_t, int8_t>>()
    {
        if (generated.size() && (history.size() == pn || coin(g)))
        {
            std::random_shuffle(generated.begin(), generated.end());
            std::pair<uint16_t, int8_t> re = std::move(generated.back());
            generated.pop_back();
            return { true, std::move(re) };
        }
        if (history.size() == pn)
            return { false, {} };
        uint16_t item = id(g);
        int8_t update = ud(g);
        if constexpr (ST == stream_type::general)
            // ignore zero update
            while (!update)
                update = ud(g);
        if (history.count(item) || history.size() < nn)
            generated.emplace_back(item, -update);
        else
            left.emplace_back(item, update);
        history.insert(item);
        return { true, { item, update } };
    }

    operator std::vector<std::pair<uint16_t, int8_t>>() const noexcept
    {
        return left;
    }
private:
    friend struct int_sparse_stream_tester;

    static std::random_device rd;

    uint64_t const pn, nn;

    std::mt19937 g;
    std::bernoulli_distribution coin;
    std::uniform_int_distribution<uint16_t> id;
    std::uniform_int_distribution<int8_t> ud;

    std::unordered_set<uint16_t> history;
    std::vector<std::pair<uint16_t, int8_t>> generated, left;

    int_sparse_stream(uint64_t pn, uint64_t nn, int8_t umin) : ud(umin), id(1), g(rd()), nn(nn), pn(pn)
    {
        if (nn > pn)
            throw std::invalid_argument("invalid sparse stream");
        generated.reserve(nn);
        left.reserve(pn - nn);
    }

    int_sparse_stream(int_sparse_stream const &) = delete;
    int_sparse_stream(int_sparse_stream &&) = delete;
    int_sparse_stream operator=(int_sparse_stream const &) = delete;
    int_sparse_stream operator=(int_sparse_stream &&) = delete;
};

template<>
int_sparse_stream<stream_type::turnstile>::int_sparse_stream(uint64_t pn, uint64_t nn) : int_sparse_stream(pn, nn, 1) {}

// -(-128) is 0 for int8_t so ignore it
template<>
int_sparse_stream<stream_type::general>::int_sparse_stream(uint64_t pn, uint64_t nn) : int_sparse_stream(pn, nn, std::numeric_limits<int8_t>::min() + 1) {}

template<stream_type ST>
std::random_device int_sparse_stream<ST>::rd {};

}

#endif
