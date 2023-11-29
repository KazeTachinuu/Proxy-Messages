#define BOOST_BIND_GLOBAL_PLACEHOLDERS
#include <iostream>
#include <thread>
#include <chrono>
#include "BasicUser.hpp"

int main() {
    try {
        // Create a BasicUser instance
        BasicUser user;

        // Start the user
        std::cout << "Test Case 1: Manage absence of correspondent\n";
        user.start();


        // After 35 seconds, the user should automatically disconnect

    } catch (const boost::system::system_error &e) {
        std::cerr << "ERROR: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
