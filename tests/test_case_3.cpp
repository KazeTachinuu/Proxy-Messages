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
        std::cout << "Test Case 3: Manage bidirectional communication\n";
        BasicUser user("0");
        user.start();
    }
    catch (const std::exception &e)
    {
        std::cerr << "ERROR: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
