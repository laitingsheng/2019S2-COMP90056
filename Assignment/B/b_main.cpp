#include <boost/program_options.hpp>

#include <cstdint>
#include <iostream>
#include <random>
#include <utility>

#include "recovery.h"
#include "stream.hpp"

int main(int argc, char *argv[])
{
    boost::program_options::options_description cli("Options");
    cli.add_options()
    ("help,h", "help")
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

    uint16_t s = 500;
    if (options.count("size"))
        s = options["size"].as<uint16_t>();

    uint16_t t = 100;
    if (options.count("trial"))
        t = options["trial"].as<uint16_t>();

    for (uint8_t sparsity = 0; sparsity < 5; ++sparsity)
    {
        uint16_t ns = s - sparsity;

        std::vector<recovery::sparse_1> sparses(t);

        std::unordered_map<uint16_t, uint64_t> record;
        stream::turnstile_stream<uint16_t, int8_t> ts(s, ns);
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
        while (record.size() < sparsity)
        {
            stream::turnstile_stream<uint16_t, int8_t> ts(s, ns);
            while (record.size() < sparsity)
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
        }

        uint16_t correct = 0;
        for (auto const & sparse: sparses)
        {
            std::pair<uint16_t, uint64_t> output = sparse;
            switch (sparsity)
            {
                case 0:
                    correct += output.first == 0 && output.second == 0;
                    break;
                case 1:
                    {
                        std::vector<std::pair<uint16_t, int8_t>> left = ts;
                        auto const & [item, update] = left.back();
                        correct += output.first == item && output.second == uint8_t(-update);
                    }
                    break;
                default:
                    correct += output.first == 2 && output.second == 0;
                    break;
            }
        }
        std::cout << "Correct rate (" << s << " : " << ns << "): " << correct << "/" << t << std::endl;
    }

    return 0;
}
