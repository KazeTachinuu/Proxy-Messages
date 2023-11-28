// BasicUser.hpp

#ifndef BASIC_USER_HPP
#define BASIC_USER_HPP

#include <string>
#include <memory>
#include <boost/asio.hpp>

class BasicUser
{
public:
    BasicUser(const std::string &secret);

    void start();

private:
    void handleCommunication();

    boost::asio::io_context io_context_;
    boost::asio::ip::tcp::socket socket_;
    std::string secret_;
    boost::asio::streambuf buffer;
};

#endif // BASIC_USER_HPP
