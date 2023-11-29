// BasicUser.hpp

#ifndef BASIC_USER_HPP
#define BASIC_USER_HPP

#include <boost/asio.hpp>

class BasicUser
{
public:
    BasicUser();

    void start();

private:
    void startRead();

    boost::asio::io_context io_context_;
    boost::asio::ip::tcp::socket socket_;
    boost::asio::streambuf receiveBuffer_;
};

#endif // BASIC_USER_HPP
