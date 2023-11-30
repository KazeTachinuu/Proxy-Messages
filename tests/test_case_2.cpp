#define BOOST_BIND_GLOBAL_PLACEHOLDERS
#include <boost/program_options.hpp>
#include <iostream>
#include <thread>

#include "BasicUser.hpp"

namespace po = boost::program_options;

int main(int argc, char *argv[])
{
    try
    {
        po::options_description desc("Allowed options");
        desc.add_options()("help", "produce help message")(
            "User", po::value<std::string>()->required(),
            "specify the user (A/B)");

        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);

        if (vm.count("help"))
        {
            std::cout << desc << std::endl;
            return 1;
        }

        po::notify(vm);

        std::string userOption = vm["User"].as<std::string>();
        if (userOption != "A" && userOption != "B")
        {
            std::cerr << "ERROR: Invalid user option. Please use --User A/B\n";
            return 1;
        }

        BasicUser user("0");

        std::thread userThread([&]() { user.start(); });
        // Wait for user A to connect to the proxy
        std::this_thread::sleep_for(std::chrono::seconds(1));

        std::cout << "Test Case 2: Manage basic communication\n";
        if (userOption == "A")
        {
            std::cout << "Handling User A\n";
            std::this_thread::sleep_for(std::chrono::seconds(5));

            user.sendMessage("[MSG]hello");
        }
        else
        {
            std::cout << "Handling User B\n";
        }

        userThread.join();
    }
    catch (const std::exception &e)
    {
        std::cerr << "ERROR: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
