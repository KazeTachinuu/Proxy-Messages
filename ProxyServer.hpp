#pragma once

#include <string>

class ProxyServer
{
public:
    explicit ProxyServer(const std::string &secret);

    void start();

private:
    std::string secret;
};
