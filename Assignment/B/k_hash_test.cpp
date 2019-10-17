#define BOOST_TEST_MODULE KHash Unit Test

#include <cstdint>
#include <limits>
#include <random>

#include <boost/test/included/unit_test.hpp>

#include "k_hash.hpp"

struct k_hash_tester final
{
    void test_hash()
    {
        k_hash<std::uint8_t> h(32, 50);

        // integer hash is trivial and perfect
        constexpr uint8_t item = 3;
        BOOST_REQUIRE_EQUAL(h.basic(item), item);

        std::uint64_t re = h(item), exp = 0, acc = 1;
        for (uint8_t i = 0; i < h.k; ++i)
        {
            exp += h.coeffs[i] * acc;
            acc *= item;
        }
        exp = (exp + h.a * acc) % h.p;
        BOOST_CHECK_EQUAL(re, exp);
    }
};

struct k_hash_test_fixture
{
    k_hash_tester tester;
};

BOOST_FIXTURE_TEST_SUITE(KHash, k_hash_test_fixture)

BOOST_AUTO_TEST_CASE(Hashing)
{
    for (auto i = 0; i < 10; ++i)
        tester.test_hash();
}

BOOST_AUTO_TEST_SUITE_END()
