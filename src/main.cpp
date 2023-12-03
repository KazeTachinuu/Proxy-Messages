#include <boost/program_options.hpp>
#include <iostream>

#include "BasicUser.hpp"
#include "ProxyServer.hpp"

namespace po = boost::program_options;

void printHelp(const po::options_description &desc,
               const po::options_description &hidden)
{
    std::cout << "Usage: ./main --mode User/Proxy --secret <secret>\n"
              << desc << std::endl;
    std::cout << hidden << std::endl;
}

void printVersion()
{
    std::cout << "Snowpack Proxy Test Version 3.5" << std::endl;
}

int main(int argc, char *argv[])
{
    po::options_description desc("Program options");
    desc.add_options()("mode,m",
                       po::value<std::string>()->default_value("Proxy"),
                       "Mode to run the program in. (Proxy/User)")(
        "secret,s", po::value<std::string>()->default_value("0"),
        "Secret to use for the proxy server.");

    po::options_description hidden("Info options");
    hidden.add_options()("version,V", "Print version information")(
        "help,h", "Print help message")(
        "port,p", po::value<std::string>()->default_value("12345"),
        "Port to use for the proxy server.")(
        "ip,i", po::value<std::string>()->default_value("127.0.0.1"),
        "IP address to use for the proxy server.");
    po::variables_map vm;

    try
    {
        // Parse command line arguments with also hidden options
        po::options_description all_options;
        all_options.add(desc).add(hidden);
        po::store(po::parse_command_line(argc, argv, all_options), vm);
        po::notify(vm);

        if (argc == 1 || vm.count("help"))
        {
            printHelp(desc, hidden);
            return 0;
        }

        if (vm.count("version"))
        {
            printVersion();
            return 0;
        }
    }
    catch (po::error &e)
    {
        std::cerr << "ERROR: " << e.what() << "\n";
        printHelp(desc, hidden);
        return 1;
    }

    std::string mode = vm["mode"].as<std::string>();
    std::string channel = vm["secret"].as<std::string>();
    std::string port = vm["port"].as<std::string>();
    std::string ip = vm["ip"].as<std::string>();

    try
    {
        if (mode == "Proxy")
        {
            ProxyServer proxy(port);
            proxy.start();
        }
        else if (mode == "User")
        {
            BasicUser user(ip, port, channel);
            user.start();
        }
        else
        {
            throw po::validation_error(
                po::validation_error::invalid_option_value, "mode");
        }
    }
    catch (const po::validation_error &e)
    {
        std::cerr << "ERROR: Invalid mode. Use --mode Proxy or --mode User.\n";
        printHelp(desc, hidden);
        return 1;
    }
    catch (const boost::system::system_error &e)
    {
        if (e.code() == boost::asio::error::connection_refused)
        {
            std::cerr << "ERROR: Connection refused. The proxy server may not "
                         "be running yet.\n";
        }
        else
        {
            std::cerr << "ERROR: " << e.what() << "\n";
        }
        return 1;
    }
    catch (const std::exception &e)
    {
        std::cerr << "ERROR: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
