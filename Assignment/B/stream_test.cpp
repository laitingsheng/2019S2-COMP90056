#define BOOST_TEST_MODULE Stream Generator Unit Test

#include <cstdint>
#include <stdexcept>
#include <string>
#include <unordered_map>

#include <boost/test/included/unit_test.hpp>

#include "stream.hpp"

namespace stream
{

struct int_sparse_stream_tester final
{
    template<stream_type ST>
    void test_generation(uint64_t const pn, uint64_t const nn)
    {
        auto g = int_sparse_stream<ST>(pn, nn);
        std::unordered_map<uint16_t, int64_t> record;
        while (true)
        {
            std::pair<bool, std::pair<uint16_t, int8_t>> o = g;
            if (!o.first)
                break;
            auto const & [item, update] = o.second;
            BOOST_REQUIRE_NE(update, 0);
            if constexpr (ST == stream_type::turnstile)
                if (update < 0)
                    BOOST_REQUIRE_GE(record[item], -update);
            record[item] += update;
        }
        BOOST_REQUIRE_EQUAL(g.pn, 0);
        BOOST_REQUIRE_EQUAL(g.nn, 0);
        uint64_t count = 0;
        for (auto const & [k, v] : record)
            count += v != 0;
        BOOST_REQUIRE_LE(count, pn - nn);
        BOOST_REQUIRE_EQUAL(g.generated.size(), pn - nn);
    }
};

}

struct int_sparse_stream_test_fixture
{
    stream::int_sparse_stream_tester tester;
};

bool catch_invalid(std::invalid_argument const & e) noexcept
{
    using namespace std::string_literals;
    return "invalid sparse stream"s == e.what();
}

BOOST_FIXTURE_TEST_SUITE(IntSparseStream, int_sparse_stream_test_fixture)

BOOST_AUTO_TEST_CASE(Turnstile)
{
    BOOST_REQUIRE_EXCEPTION(tester.test_generation<stream::stream_type::turnstile>(2, 3), std::invalid_argument, catch_invalid);
    BOOST_TEST_MESSAGE("0 sparse");
    for (uint8_t i = 0; i < 10; ++i)
        tester.test_generation<stream::stream_type::turnstile>(100, 100);
    BOOST_TEST_MESSAGE("1 sparse");
    for (uint8_t i = 0; i < 10; ++i)
        tester.test_generation<stream::stream_type::turnstile>(100, 99);
    BOOST_TEST_MESSAGE("2 sparse");
    for (uint8_t i = 0; i < 10; ++i)
        tester.test_generation<stream::stream_type::turnstile>(100, 98);
    BOOST_TEST_MESSAGE("3 sparse");
    for (uint8_t i = 0; i < 10; ++i)
        tester.test_generation<stream::stream_type::turnstile>(100, 97);
    BOOST_TEST_MESSAGE("4 sparse");
    for (uint8_t i = 0; i < 10; ++i)
        tester.test_generation<stream::stream_type::turnstile>(100, 96);
}

BOOST_AUTO_TEST_CASE(General)
{
    BOOST_REQUIRE_EXCEPTION(tester.test_generation<stream::stream_type::general>(2, 3), std::invalid_argument, catch_invalid);
    BOOST_TEST_MESSAGE("0 sparse");
    for (uint8_t i = 0; i < 10; ++i)
        tester.test_generation<stream::stream_type::general>(100, 100);
    BOOST_TEST_MESSAGE("1 sparse");
    for (uint8_t i = 0; i < 10; ++i)
        tester.test_generation<stream::stream_type::general>(100, 99);
    BOOST_TEST_MESSAGE("2 sparse");
    for (uint8_t i = 0; i < 10; ++i)
        tester.test_generation<stream::stream_type::general>(100, 98);
    BOOST_TEST_MESSAGE("3 sparse");
    for (uint8_t i = 0; i < 10; ++i)
        tester.test_generation<stream::stream_type::general>(100, 97);
    BOOST_TEST_MESSAGE("4 sparse");
    for (uint8_t i = 0; i < 10; ++i)
        tester.test_generation<stream::stream_type::general>(100, 96);
}

BOOST_AUTO_TEST_SUITE_END()
