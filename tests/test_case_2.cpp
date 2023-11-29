#define BOOST_BIND_GLOBAL_PLACEHOLDERS
#include <iostream>
#include <thread> // Add necessary headers

#include "BasicUser.hpp"

int main() {
    try {


        // Create and start user A
        BasicUser userA;
        std::thread userAThread([&]() { userA.start(); });

        // Wait for user A to connect to the proxy
        std::this_thread::sleep_for(std::chrono::seconds(1));

        // Create and start user B
        BasicUser userB;
        std::thread userBThread([&]() { userB.start(); });

        // Wait for user B to connect to the proxy
        std::this_thread::sleep_for(std::chrono::seconds(1));

        // Perform the communication between A and B (you need to implement this part)
        userA.sendMessage("[MSG]hello");

        // Wait for the threads to finish
        userAThread.join();
        userBThread.join();
    } catch (const std::exception &e) {
        std::cerr << "ERROR: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
