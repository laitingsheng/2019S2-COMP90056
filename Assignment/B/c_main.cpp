#include <boost/program_options.hpp>

#include <cstdint>
#include <iostream>
#include <random>
#include <unordered_map>
#include <utility>

#include "recovery.h"
#include "stream.hpp"

int main(int argc, char *argv[])
{
    boost::program_options::options_description cli("Options");
    cli.add_options()
    ("help,h", "help")
    ("delta,d", boost::program_options::value<double>(), "delta parameter for k-sparse, defaults to 0.1")
    ("k-sparse,k", boost::program_options::value<uint16_t>(), "sparsity parameter for k-sparse, defaults to 10")
    ("l-wise,l", boost::program_options::value<uint8_t>(), "l-wise independent universal hash, defaults to 2")
    ("size,s", boost::program_options::value<uint16_t>(), "stream size of positive update, must be positive, defaults to 500")
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

    uint16_t k = 10;
    if (options.count("k-sparse"))
        k = options["k-sparse"].as<uint16_t>();

    uint8_t l = 2;
    if (options.count("l-wise"))
        l = options["l-wise"].as<uint8_t>();

    uint16_t s = 500;
    if (options.count("size"))
        s = options["size"].as<uint16_t>();

    uint16_t t = 100;
    if (options.count("trial"))
        t = options["trial"].as<uint16_t>();

    std::cout << "Turnstile Stream" << std::endl;
    for (uint8_t sparsity = 0; sparsity <= 2 * k; ++sparsity)
    {
        uint16_t ns = s - sparsity;

        std::vector<recovery::sparse_k> sparses;
        sparses.reserve(t);
        for (uint16_t i = 0; i < t; ++i)
            sparses.emplace_back(k, d, l);

        std::unordered_map<uint16_t, int64_t> record;
        stream::int_sparse_stream<stream::stream_type::turnstile> ts(s, ns);
        while (true)
        {
            std::pair<bool, std::pair<uint16_t, int8_t>> r = ts;
            if (!r.first)
                break;
            auto const & [item, update] = r.second;
            for (auto & sparse : sparses)
                sparse(item, update);
            record[item] += update;
            if (!record[item])
                record.erase(item);
        }

        uint16_t correct = 0;
        for (auto const & sparse: sparses)
        {
            std::unordered_map<uint16_t, int64_t> output = sparse;
            if (sparsity == 0 || sparsity > k)
                correct += output.size() == 0;
            else
                correct += output == record;
        }
        std::cout << "    Correct rate (" << s << " : " << ns << "): " << correct << "/" << t << std::endl;
    }

    std::cout << "General Stream" << std::endl;
    for (uint8_t sparsity = 0; sparsity <= 2 * k; ++sparsity)
    {
        uint16_t ns = s - sparsity;

        std::vector<recovery::sparse_k> sparses;
        sparses.reserve(t);
        for (uint16_t i = 0; i < t; ++i)
            sparses.emplace_back(k, d, l);

        std::unordered_map<uint16_t, int64_t> record;
        stream::int_sparse_stream<stream::stream_type::general> gs(s, ns);
        while (true)
        {
            std::pair<bool, std::pair<uint16_t, int8_t>> r = gs;
            if (!r.first)
                break;
            auto const & [item, update] = r.second;
            for (auto & sparse : sparses)
                sparse(item, update);
            record[item] += update;
            if (!record[item])
                record.erase(item);
        }

        uint16_t correct = 0;
        for (auto const & sparse: sparses)
        {
            std::unordered_map<uint16_t, int64_t> output = sparse;
            if (sparsity == 0 || sparsity > k)
                correct += output.size() == 0;
            else
                correct += output == record;
        }
        std::cout << "    Correct rate (" << s << " : " << ns << "): " << correct << "/" << t << std::endl;
    }

    return 0;
}
