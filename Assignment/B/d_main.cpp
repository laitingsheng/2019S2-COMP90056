#include <cstdint>

#include <iostream>
#include <limits>

#include <boost/program_options.hpp>

#include "sampler.hpp"
#include "stream.hpp"

int main(int argc, char * argv[])
{
    boost::program_options::options_description cli("Options");
    cli.add_options()
    ("help,h", "help")
    ("delta,d", boost::program_options::value<double>(), "delta parameter, affect the sparsity and the number of independent variables in the universal hash family, defaults to 0.1")
    ("size,s", boost::program_options::value<uint16_t>(), "stream size, must be positive, defaults to 500")
    ("trial,t", boost::program_options::value<uint16_t>(), "number of trials, must be positive, defaults to 100");

    boost::program_options::variables_map options;
    boost::program_options::store(boost::program_options::parse_command_line(argc, argv, cli), options);
    boost::program_options::notify(options);

    if (options.count("help"))
    {
        std::cout << cli << std::endl;
        return 0;
    }

    double d = 0.1;
    if (options.count("delta"))
    {
        d = options["delta"].as<double>();
        if (d <= 0 || d >= 1)
        {
            std::cerr << "-d requires a value in range (0, 1)" << std::endl;
            return 1;
        }
    }

    uint16_t s = 500;
    if (options.count("size"))
        s = options["size"].as<uint16_t>();

    uint16_t t = 100;
    if (options.count("trial"))
        t = options["trial"].as<uint16_t>();

    for (uint16_t sparsity = 50; sparsity < 60; ++sparsity)
    {
        uint16_t ns = s - sparsity;

        std::vector<sampler::l0_general> l0s;
        l0s.reserve(t);
        for (uint16_t i = 0; i < t; ++i)
            l0s.emplace_back(std::numeric_limits<uint16_t>::max(), d);

        std::unordered_map<uint16_t, int64_t> record;
        stream::int_sparse_stream<stream::stream_type::turnstile> ts(s, ns);
        while (true)
        {
            std::pair<bool, std::pair<uint16_t, int8_t>> r = ts;
            if (!r.first)
                break;
            auto const & [item, update] = r.second;
            for (auto & l0 : l0s)
                l0(item, update);
            record[item] += update;
            if (!record[item])
                record.erase(item);
        }

        std::unordered_map<uint16_t, int64_t> samples;
        uint16_t failure = 0;
        for (auto const & l0 : l0s)
        {
            std::pair<bool, uint16_t> re = l0;
            if (re.first)
                ++samples[re.second];
            else
                ++failure;
        }
        std::cout << failure << std::endl;
        for (auto const & [k, v] : record)
            std::cout << k << " " << v << " " << samples[k] << std::endl;
    }

    for (uint16_t sparsity = 50; sparsity < 60; ++sparsity)
    {
        uint16_t ns = s - sparsity;

        std::vector<sampler::l0_general> l0s;
        l0s.reserve(t);
        for (uint16_t i = 0; i < t; ++i)
            l0s.emplace_back(std::numeric_limits<uint16_t>::max(), d);

        std::unordered_map<uint16_t, int64_t> record;
        stream::int_sparse_stream<stream::stream_type::general> ts(s, ns);
        while (true)
        {
            std::pair<bool, std::pair<uint16_t, int8_t>> r = ts;
            if (!r.first)
                break;
            auto const & [item, update] = r.second;
            for (auto & l0 : l0s)
                l0(item, update);
            record[item] += update;
            if (!record[item])
                record.erase(item);
        }

        std::unordered_map<uint16_t, int64_t> samples;
        uint16_t failure = 0;
        for (auto const & l0 : l0s)
        {
            std::pair<bool, uint16_t> re = l0;
            if (re.first)
                ++samples[re.second];
            else
                ++failure;
        }
        std::cout << failure << std::endl;
        for (auto const & [k, v] : record)
            std::cout << k << " " << v << " " << samples[k] << std::endl;
    }

    return 0;
}
