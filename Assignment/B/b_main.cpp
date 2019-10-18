int main()
{
    boost::program_options::options_description cli("Options");
    cli.add_options()
    ("help,h", "help")
    ("range,r", boost::program_options::value<uint16_t>(), "data range, must be positive, defaults to 100")
    ("size,s", boost::program_options::value<uint64_t>(), "stream size, must be positive, defaults to 10000")
    ("trial,t", boost::program_options::value<uint64_t>(), "number of trials, must be positive, defaults to 2 * range");

    boost::program_options::variables_map options;
    boost::program_options::store(boost::program_options::parse_command_line(argc, argv, cli), options);
    boost::program_options::notify(options);

    if (options.count("help"))
    {
        std::cout << cli << std::endl;
        return 0;
    }

    uint64_t s = 10000;
    if (options.count("size"))
        s = options["size"].as<uint64_t>();

    uint64_t r = 100;
    if (options.count("range"))
        r = options["range"].as<uint64_t>();

    uint64_t t = 2 * r;
    if (options.count("trial"))
        t = options["trial"].as<uint64_t>();

    return 0;
}
