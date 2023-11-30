// BasicUser.hpp

#pragma once

#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>

namespace asio = boost::asio;

class BasicUser
{
public:
    BasicUser(const std::string &channel);

    void start();
    void sendMessage(const std::string &message);

private:
    void startRead();
    void handleRead(const boost::system::error_code &error,
                    std::size_t bytes_received);
    void handleCommandResponse(const std::string &message);
    void startDisconnectTimer();
    void stopDisconnectTimer();
    void handleDisconnect();
    void startReadUntilUserCount();

private:
    asio::io_context io_context_;
    asio::ip::tcp::socket socket_;
    asio::streambuf receiveBuffer_;
    unsigned short waitingTime_;
    asio::deadline_timer disconnectTimer_;
    std::string channel_;
};
