#include <cstdint>
#include <fstream>
#include <iostream>
#include <random>
#include <vector>

#include <boost/program_options.hpp>

#include "hash.hpp"
#include "sampler.hpp"

int main(int argc, char * argv[])
{
    boost::program_options::options_description cli("Options");
    cli.add_options()
    ("help,h", "help")
    ("epsilon,e", boost::program_options::value<double>(), "epsilon value used in l0 sampling, must be in [0, 1], defaults to 1e-8")
    ("probability,p", boost::program_options::value<double>(), "success probability for binomial distribution, must be in [0, 1], defaults to 0.5")
    ("range,r", boost::program_options::value<uint16_t>(), "data range, must be positive, defaults to the stream size / 100")
    ("size,s", boost::program_options::value<uint16_t>(), "stream size, must be positive, defaults to 10000")
    ("trial,t", boost::program_options::value<uint16_t>(), "number of trials, must be positive, defaults to 10 * range");

    boost::program_options::variables_map options;
    boost::program_options::store(boost::program_options::parse_command_line(argc, argv, cli), options);
    boost::program_options::notify(options);

    if (options.count("help"))
    {
        std::cout << cli << std::endl;
        return 0;
    }

    double e = 1e-8;
    if (options.count("epsilon"))
    {
        e = options["epsilon"].as<double>();
        if (e <= 0 || e >= 1)
        {
            std::cerr << "-e requires a value in range (0, 1)" << std::endl;
            return 1;
        }
    }

    double p = 0.5;
    if (options.count("probability"))
    {
        p = options["probability"].as<double>();
        if (p <= 0 || p >= 1)
        {
            std::cerr << "-p requires a value in range (0, 1)" << std::endl;
            return 1;
        }
    }

    uint16_t s = 10000;
    if (options.count("size"))
        s = options["size"].as<uint16_t>();

    uint16_t r = s / 100;
    if (options.count("range"))
        r = options["range"].as<uint16_t>();

    uint16_t t = 10 * r;
    if (options.count("trial"))
        t = options["trial"].as<uint16_t>();

    // used to record distribution
    std::vector<uint16_t> vs(r), vlk(r);
    std::mt19937_64 g { std::random_device()() };
    uint8_t const k = log2(floor(1 / e));
    hash::k_universal_family<uint16_t> kuf(k);

    std::uniform_int_distribution<uint16_t> ud(0, r - 1);
    std::vector<sampler::l0_insertion> l0ks;
    l0ks.reserve(t);
    for (uint64_t i = 0; i < t; ++i)
        l0ks.emplace_back(r, kuf());
    for (uint64_t i = 0; i < s; ++i)
    {
        uint64_t v = ud(g);
        ++vs[v];
        for (auto & l0k : l0ks)
            l0k += v;
    }
    for (auto const & l0k : l0ks)
        ++vlk[l0k];
    for (uint64_t i = 0; i < r; ++i)
        std::cout << i << " " << vs[i] << " " << vlk[i] << std::endl;

    vs = std::vector<uint16_t>(r);
    vlk = std::vector<uint16_t>(r);

    std::binomial_distribution<uint16_t> ub(r - 1, p);
    l0ks.clear();
    for (uint64_t i = 0; i < t; ++i)
        l0ks.emplace_back(r, kuf());
    for (uint64_t i = 0; i < s; ++i)
    {
        uint64_t v = ub(g);
        ++vs[v];
        for (auto & l0k : l0ks)
            l0k += v;
    }
    for (auto const & l0k : l0ks)
        ++vlk[l0k];
    for (uint64_t i = 0; i < r; ++i)
        std::cout << i << " " << vs[i] << " " << vlk[i] << std::endl;

    return 0;
}
