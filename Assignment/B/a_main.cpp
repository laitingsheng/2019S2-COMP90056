#include <cstdint>
#include <fstream>
#include <iostream>
#include <random>
#include <vector>

#include<boost/program_options.hpp>

#include "l.hpp"

int main(int argc, char *argv[])
{
    boost::program_options::options_description cli("Options");
    cli.add_options()
    ("help,h", "help")
    ("epsilon,e", boost::program_options::value<double>(), "epsilon value used in l0 sampling, must be in [0, 1], defaults to 0.1")
    ("index,i", boost::program_options::value<std::uint64_t>(), "the index of this run, must be positive, defaults to 0 (start)")
    ("probability,p", boost::program_options::value<double>(), "success probability for binomial distribution, must be in [0, 1], defaults to 0.5")
    ("range,r", boost::program_options::value<std::uint64_t>(), "data range, must be positive, defaults to 100")
    ("size,s", boost::program_options::value<std::uint64_t>(), "stream size, must be positive, defaults to 10000")
    ("trial,t", boost::program_options::value<std::uint64_t>(), "number of trials, must be positive, defaults to double the range");

    boost::program_options::variables_map options;
    boost::program_options::store(boost::program_options::parse_command_line(argc, argv, cli), options);
    boost::program_options::notify(options);

    if (options.count("help"))
    {
        std::cout << cli << std::endl;
        return 0;
    }

    double e = 0.1, p = 0.5;
    if (options.count("epsilon"))
    {
        e = options["epsilon"].as<double>();
        if (e <= 0 || p >= 1)
        {
            std::cout << "-e requires a value in range [0, 1]" << std::endl;
            return 1;
        }
    }

    if (options.count("probability"))
    {
        p = options["probability"].as<double>();
        if (p <= 0 || p >= 1)
        {
            std::cout << "-p requires a value in range [0, 1]" << std::endl;
            return 1;
        }
    }

    std::uint64_t r = 100, s = 10000;
    if (options.count("range"))
        r = options["range"].as<std::uint64_t>();
    if (options.count("size"))
        s = options["size"].as<std::uint64_t>();
    std::uint64_t t = 2 * r;
    if (options.count("trial"))
        t = options["trial"].as<std::uint64_t>();

    // used to record distribution
    std::vector<uint64_t> vs(r + 1), vl(r + 1);
    std::mt19937_64 g { std::random_device()() };

    std::cout << "Running Uniform Distribution" << std::endl;
    std::uniform_int_distribution<std::uint64_t> ud(0, r);
    std::vector<l0_insertion_sampler<std::uint64_t>> ul0s;
    ul0s.reserve(t);
    for (std::uint64_t i = 0; i < t; ++i)
        ul0s.push_back(l0_insertion_sampler<std::uint64_t>(r, e));
    for (std::uint64_t i = 0; i < s; ++i)
    {
        std::uint64_t v = ud(g);
        ++vs[v];
        #pragma omp parallel for
        for (std::uint64_t j = 0; j < t; ++j)
            ul0s[j] += v;
    }
    for (std::uint64_t i = 0; i < t; ++i)
        ++vl[ul0s[i]];
    for (std::uint64_t i = 0; i <= r; ++i)
        std::cout << "    " << vs[i] << " " << vl[i] << std::endl;

    vs.clear();
    vl.clear();
    std::cout << std::endl;

    std::cout << "Running Binomial Distribution" << std::endl;
    std::binomial_distribution<std::uint64_t> ub(r, p);
    std::vector<l0_insertion_sampler<std::uint64_t>> bl0s;
    bl0s.reserve(t);
    for (std::uint64_t i = 0; i < t; ++i)
        bl0s.push_back(l0_insertion_sampler<std::uint64_t>(r, e));
    for (std::uint64_t i = 0; i < s; ++i)
    {
        std::uint64_t v = ub(g);
        ++vs[v];
        #pragma omp parallel for
        for (std::uint64_t j = 0; j < t; ++j)
            bl0s[j] += v;
    }
    for (std::uint64_t i = 0; i < t; ++i)
        ++vl[bl0s[i]];
    for (std::uint64_t i = 0; i <= r; ++i)
        std::cout << "    " << vs[i] << " " << vl[i] << std::endl;

    return 0;
}
