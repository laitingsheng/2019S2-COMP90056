#include <cstdint>
#include <fstream>
#include <iostream>
#include <random>
#include <vector>

#include<boost/program_options.hpp>

#include "hash.hpp"
#include "sampler.hpp"

int main(int argc, char *argv[])
{
    boost::program_options::options_description cli("Options");
    cli.add_options()
    ("help,h", "help")
    ("epsilon,e", boost::program_options::value<double>(), "epsilon value used in l0 sampling, must be in [0, 1], defaults to 1e-32")
    ("probability,p", boost::program_options::value<double>(), "success probability for binomial distribution, must be in [0, 1], defaults to 0.5")
    ("range,r", boost::program_options::value<uint64_t>(), "data range, must be positive, defaults to the stream size")
    ("size,s", boost::program_options::value<uint64_t>(), "stream size, must be positive, defaults to 10000")
    ("trial,t", boost::program_options::value<uint64_t>(), "number of trials, must be positive, defaults to range / 10");

    boost::program_options::variables_map options;
    boost::program_options::store(boost::program_options::parse_command_line(argc, argv, cli), options);
    boost::program_options::notify(options);

    if (options.count("help"))
    {
        std::cout << cli << std::endl;
        return 0;
    }

    double e = 1e-32;
    if (options.count("epsilon"))
    {
        e = options["epsilon"].as<double>();
        if (e <= 0 || e >= 1)
        {
            std::cerr << "-e requires a value in range [0, 1]" << std::endl;
            return 1;
        }
    }

    double p = 0.5;
    if (options.count("probability"))
    {
        p = options["probability"].as<double>();
        if (p <= 0 || p >= 1)
        {
            std::cerr << "-p requires a value in range [0, 1]" << std::endl;
            return 1;
        }
    }

    uint64_t s = 10000;
    if (options.count("size"))
        s = options["size"].as<uint64_t>();

    uint64_t r = s;
    if (options.count("range"))
        r = options["range"].as<uint64_t>();

    uint64_t t = r / 10;
    if (options.count("trial"))
        t = options["trial"].as<uint64_t>();

    using ItemType = uint64_t;

    // used to record distribution
    std::vector<ItemType> vs(r + 1), vl(r + 1);
    std::mt19937_64 g { std::random_device()() };
    uint8_t const k = log(floor(1 / e));
    hash::k_universal_family<ItemType> kuf(k);

    std::uniform_int_distribution<ItemType> ud(0, r);
    std::vector<sampler::l0_insertion<ItemType>> ul0s;
    ul0s.reserve(t);
    for (uint64_t i = 0; i < t; ++i)
        ul0s.emplace_back(r, kuf());
    for (uint64_t i = 0; i < s; ++i)
    {
        uint64_t v = ud(g);
        ++vs[v];
        for (uint64_t j = 0; j < t; ++j)
            ul0s[j] += v;
    }
    for (auto const & ul0 : ul0s)
        ++vl[ul0];
    for (uint64_t i = 0; i <= r; ++i)
        std::cout << i << " " << vs[i] << " " << vl[i] << std::endl;

    vs.clear();
    vl.clear();

    std::binomial_distribution<ItemType> ub(r, p);
    std::vector<sampler::l0_insertion<ItemType>> bl0s;
    bl0s.reserve(t);
    for (uint64_t i = 0; i < t; ++i)
        bl0s.emplace_back(r, kuf());
    for (uint64_t i = 0; i < s; ++i)
    {
        uint64_t v = ub(g);
        ++vs[v];
        for (uint64_t j = 0; j < t; ++j)
            bl0s[j] += v;
    }
    for (auto const & bl0 : bl0s)
        ++vl[bl0];
    for (uint64_t i = 0; i <= r; ++i)
        std::cout << i << " " << vs[i] << " " << vl[i] << std::endl;

    return 0;
}
