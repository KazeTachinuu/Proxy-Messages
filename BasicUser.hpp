
#pragma once

#include <string>

class BasicUser
{
public:
    explicit BasicUser(const std::string &secret);

    void start();

private:
    std::string secret;
};
