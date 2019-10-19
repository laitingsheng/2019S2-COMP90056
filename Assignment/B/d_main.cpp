#include <cstdint>

#include <boost/program_options.hpp>

#include "recovery.h"
#include "stream.hpp"

int main(int argc, char * argv[])
{
    boost::program_options::options_description cli("Options");
    cli.add_options()
    ("help,h", "help")
    ("delta,d", boost::program_options::value<double>(), "delta parameter, affect the sparsity and the number of independent variables in the universal hash family, defaults to 0.1")
    ("range,r", boost::program_options::value<uint16_t>(), "data range, must be positive, defaults to the stream size / 100")
    ("size,s", boost::program_options::value<uint16_t>(), "stream size, must be positive, defaults to 10000")
    ("trial,t", boost::program_options::value<uint16_t>(), "number of trials, must be positive, defaults to 10 * range");

    boost::program_options::variables_map options;
    boost::program_options::store(boost::program_options::parse_command_line(argc, argv, cli), options);
    boost::program_options::notify(options);

    return 0;
}
