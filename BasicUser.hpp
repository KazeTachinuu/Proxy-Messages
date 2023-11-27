// BasicUser.hpp
#ifndef BASIC_USER_HPP
#define BASIC_USER_HPP

#include <string>
#include <thread>
#include <iostream>
#include <boost/asio.hpp>

class BasicUser
{
public:
    explicit BasicUser(const std::string &secret);
    void start();

private:
    void startReading(boost::asio::ip::tcp::socket &socket);
    void startUserInput(boost::asio::ip::tcp::socket &socket);

    std::string secret;
};

#endif // BASIC_USER_HPP
