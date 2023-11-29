#pragma once

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <iostream>

namespace asio = boost::asio;

class BasicUser
{
public:
    BasicUser();

    void start();

private:
    void startRead();
    void handleRead(const boost::system::error_code& error, std::size_t bytes_received);
    void handleCommandResponse(const std::string& message);
    void handleGetUserConnectedResponse(const std::string& message);
    void handleUserCountResponse(int numConnectedUsers);
    void startDisconnectTimer();
    void stopDisconnectTimer();
    void sendMessage(const std::string& message);

    asio::io_context io_context_;
    asio::ip::tcp::socket socket_;
    asio::streambuf receiveBuffer_;
    asio::deadline_timer disconnectTimer_;
};

