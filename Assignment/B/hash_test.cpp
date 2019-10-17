#define BOOST_TEST_MODULE KHash Unit Test

#include <cstdint>
#include <limits>
#include <random>

#include <boost/test/included/unit_test.hpp>

#include "hash.hpp"
#include "prime.h"

namespace hash
{

struct k_universal_tester final
{
    static void test_trivial()
    {
        // integer hash is trivial and perfect
        BOOST_REQUIRE_EQUAL(k_universal<uint8_t>::basic(item), item);
    }

    void test_hash(uint8_t k, uint8_t trials)
    {
        hash::k_universal_family<uint8_t> kuf(k, 50);

        for (uint8_t t = 1; t <= trials; ++t)
        {
            auto h = kuf();
            uint64_t re = h(item), exp = 0, acc = 1;
            for (uint32_t i : h.as)
            {
                exp += i * acc;
                acc *= item;
            }
            exp = prime::mersennes::mersenne31(exp + h.a * acc);
            BOOST_CHECK_EQUAL(re, exp);
        }
    }
private:
    static constexpr uint8_t item = 3;
};

}

struct k_universal_test_fixture
{
    hash::k_universal_tester tester;
};

BOOST_FIXTURE_TEST_SUITE(KUniversal, k_universal_test_fixture)

BOOST_AUTO_TEST_CASE(Trivial)
{
    tester.test_trivial();
}

BOOST_AUTO_TEST_CASE(Hashing)
{
    for (uint8_t k = 1; k <= 10; ++k)
        tester.test_hash(k, 10);
}

BOOST_AUTO_TEST_SUITE_END()
