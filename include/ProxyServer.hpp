#pragma once

#include <boost/asio.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/streambuf.hpp>
#include <iostream>
#include <map>
#include <vector>

#include "CommandHandler.hpp"

namespace asio = boost::asio;

enum class MessageType
{
    CMD,
    MSG,
    INFO,
    UNKNOWN
};

class ProxyServer
{
public:
    ProxyServer(unsigned short port);
    void start();
    void notifyUser(const std::shared_ptr<asio::ip::tcp::socket> &userSocket,
                    const std::string &message);
    void
    notifyAllUsers(const std::string &message,
                   const std::vector<std::shared_ptr<asio::ip::tcp::socket>>
                       &excludedSockets);
    std::size_t getUserCount() const;

private:
    void
    notifyNewUser(const std::shared_ptr<asio::ip::tcp::socket> &newUserSocket);
    void startAccept();
    MessageType getMessageType(const std::string &message);
    void handleCommunication(
        const std::shared_ptr<asio::ip::tcp::socket> &newUserSocket);
    void handleMessage(const std::shared_ptr<asio::ip::tcp::socket> &userSocket,
                       const std::string &username, const std::string &message);

private:
    asio::io_context io_context_;
    asio::ip::tcp::acceptor acceptor_;
    unsigned short port_;
    std::map<std::shared_ptr<asio::ip::tcp::socket>, std::string>
        connectedUsers_;
    std::vector<std::shared_ptr<asio::ip::tcp::socket>> userSockets_;
    asio::streambuf receiveBuffer_;
    std::unique_ptr<CommandHandler> commandHandler_;
};
