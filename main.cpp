// main.cpp
#include <boost/program_options.hpp>
#include <iostream>
#include <string>
#include "BasicUser.hpp"
#include "ProxyServer.hpp"

namespace po = boost::program_options;

int main(int argc, char *argv[])
{
    po::options_description desc("Allowed options");

    desc.add_options()("mode", po::value<std::string>()->required(), "Mode Selection Proxy/User")("secret", po::value<std::string>()->required(), "secret value");

    boost::program_options::variables_map vm;

    try
    {
        boost::program_options::store(
            boost::program_options::command_line_parser(argc, argv)
                .options(desc)
                .run(),
            vm);
        boost::program_options::notify(vm);
    }
    catch (boost::program_options::error &e)
    {
        std::cout << "ERROR: " << e.what() << "\n";
        std::cout << desc << "\n";
        return 1;
    }

    std::string mode(vm["mode"].as<std::string>());
    std::string secret(vm["secret"].as<std::string>());

    if (mode == "Proxy")
    {
        ProxyServer proxyServer(secret);
        proxyServer.start();
    }
    else if (mode == "User")
    {
        BasicUser basicUser(secret);
        basicUser.start();
    }
    else
    {
        std::cerr << "Invalid mode. Use 'Proxy' or 'User'.\n";
        return 1;
    }

    return 0;
}
