// BasicUser.hpp

#ifndef BASIC_USER_HPP
#define BASIC_USER_HPP

#include <boost/asio.hpp>

class BasicUser
{
public:
    BasicUser();

    void start();

    // Function to asynchronously receive messages
    void asyncReceive();

private:
    boost::asio::io_context io_context_;
    boost::asio::ip::tcp::socket socket_;

    // Buffer to store received messages
    std::array<char, 1024> receiveBuffer_;
};

#endif // BASIC_USER_HPP
