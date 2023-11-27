// main.cpp
#include <boost/program_options.hpp>
#include <iostream>

#include "BasicUser.hpp"
#include "ProxyServer.hpp"

namespace po = boost::program_options;

int main(int argc, char *argv[])
{
    po::options_description desc("Allowed options");

    desc.add_options()("mode", po::value<std::string>()->required(),
                       "Mode Selection Proxy/User")(
        "secret", po::value<std::string>()->required(), "Secret value");

    po::variables_map vm;

    try
    {
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);
    }
    catch (po::error &e)
    {
        std::cerr << "ERROR: " << e.what() << "\n";
        std::cerr << desc << "\n";
        return 1;
    }

    std::string mode(vm["mode"].as<std::string>());
    std::string secret(vm["secret"].as<std::string>());

    if (mode == "Proxy")
    {
        ProxyServer proxy(secret);
        proxy.start();
    }
    else if (mode == "User")
    {
        BasicUser user(secret);
        user.start();
    }
    else
    {
        std::cerr << "Invalid mode. Use --mode Proxy or --mode User.\n";
        return 1;
    }

    return 0;
}
