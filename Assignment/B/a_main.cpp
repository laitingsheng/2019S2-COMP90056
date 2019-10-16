#include <cstdint>
#include <iostream>

#include<boost/program_options.hpp>

#include "l.hpp"

int main(int argc, char *argv[])
{
    boost::program_options::options_description cli("Options");
    cli.add_options()
    ("help", "help")
    ("stream-size,s", boost::program_options::value<std::uint64_t>(), "stream size");

    boost::program_options::variables_map options;
    boost::program_options::store(boost::program_options::parse_command_line(argc, argv, cli), options);
    boost::program_options::notify(options);

    if (options.count("help"))
    {
        std::cout << cli << std::endl;
        return 0;
    }

    std::uint64_t stream_size = 10000;
    if (options.count("stream-size"))
    {
        stream_size = options["stream-size"].as<std::uint64_t>();
    }

    return 0;
}
