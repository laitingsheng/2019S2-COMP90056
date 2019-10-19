#define BOOST_TEST_MODULE Stream Generator Unit Test

#include <cstdint>
#include <stdexcept>
#include <string>
#include <unordered_map>

#include <boost/test/included/unit_test.hpp>

#include "stream.hpp"

namespace stream
{

struct turnstile_stream_tester final
{
    void test_generation(uint64_t pn, uint64_t nn)
    {
        auto g = turnstile_stream<uint16_t, int8_t>(pn, nn);
        uint64_t const diff = pn - nn;
        std::unordered_map<uint16_t, uint64_t> record;
        while (true)
        {
            std::pair<bool, std::pair<uint16_t, int8_t>> o = g;
            if (!o.first)
                break;
            auto const & [item, update] = o.second;
            BOOST_REQUIRE_NE(update, 0);
            if (update < 0)
            {
                BOOST_REQUIRE_GT(nn, 0);
                BOOST_REQUIRE_GE(record[item], uint64_t(-update));
                --nn;
            }
            else
            {
                BOOST_REQUIRE_GT(pn, 0);
                --pn;
            }
            record[item] += update;
        }
        BOOST_REQUIRE_EQUAL(pn, 0);
        BOOST_REQUIRE_EQUAL(nn, 0);
        uint64_t count = 0;
        for (auto const & [k, v] : record)
            count += v > 0;
        BOOST_REQUIRE_EQUAL(diff, count);
        BOOST_REQUIRE_EQUAL(g.generated.size(), diff);
    }
};

}

struct turnstile_stream_test_fixture
{
    stream::turnstile_stream_tester tester;
};

bool catch_invalid(std::invalid_argument const & e) noexcept
{
    using namespace std::string_literals;
    return "invalid turnstile stream"s == e.what();
}

BOOST_FIXTURE_TEST_SUITE(TurnstileStream, turnstile_stream_test_fixture)

BOOST_AUTO_TEST_CASE(Generation)
{
    BOOST_REQUIRE_EXCEPTION(tester.test_generation(2, 3), std::invalid_argument, catch_invalid);
    for (uint8_t i = 0; i < 10; ++i)
        tester.test_generation(100, 100);
    for (uint8_t i = 0; i < 10; ++i)
        tester.test_generation(100, 99);
    for (uint8_t i = 0; i < 10; ++i)
        tester.test_generation(100, 98);
    for (uint8_t i = 0; i < 10; ++i)
        tester.test_generation(100, 97);
    for (uint8_t i = 0; i < 10; ++i)
        tester.test_generation(100, 96);
}

BOOST_AUTO_TEST_SUITE_END()
