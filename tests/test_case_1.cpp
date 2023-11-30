#define BOOST_BIND_GLOBAL_PLACEHOLDERS
#include <chrono>
#include <iostream>
#include <thread>

#include "BasicUser.hpp"

int main()
{
    try
    {
        // Create a BasicUser instance
        BasicUser user("0");

        // Start the user
        std::cout << "Test Case 1: Manage absence of correspondent\n";
        user.start();
    }
    catch (const boost::system::system_error &e)
    {
        std::cerr << "ERROR: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
