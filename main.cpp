// main.cpp
#include <iostream>
#include "ProxyServer.hpp"
#include "BasicUser.hpp"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " --mode <Proxy/User> --secret <secretvalue>" << std::endl;
        return 1;
    }

    std::string mode;
    std::string secret;

    for (int i = 1; i < argc; i += 2) {
        std::string arg = argv[i];
        if (arg == "--mode") {
            mode = argv[i + 1];
        } else if (arg == "--secret") {
            secret = argv[i + 1];
        }
    }

    if (mode.empty() || (mode != "Proxy" && mode != "User") || secret.empty()) {
        std::cerr << "Invalid command line arguments. Please provide valid arguments." << std::endl;
        return 1;
    }

    if (mode == "Proxy") {
        ProxyServer proxy(secret);
        proxy.start();
    } else {
        BasicUser user(secret);
        user.start();
    }

    return 0;
}

