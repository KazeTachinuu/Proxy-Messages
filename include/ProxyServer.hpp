#pragma once

#include <boost/asio.hpp>
#include <iostream>
#include <vector>
#include <memory>

namespace asio = boost::asio;

enum class MessageType
{
    CMD,
    MSG,
    INFO,
    UNKNOWN
};

class CommandHandler; // Forward declaration

class ProxyServer
{
public:
    ProxyServer(unsigned short port);

    void start();

    void notifyUser(const std::shared_ptr<asio::ip::tcp::socket>& userSocket, const std::string& message);
    void notifyAllUsers(const std::string& message, const std::vector<std::shared_ptr<asio::ip::tcp::socket>>& excludedSockets);
    std::size_t getUserCount() const;

private:
    void notifyNewUser(const std::shared_ptr<asio::ip::tcp::socket>& newUserSocket);
    void startAccept();
    void handleCommunication(const std::shared_ptr<asio::ip::tcp::socket>& newUserSocket);
    void handleMessage(const std::shared_ptr<asio::ip::tcp::socket>& userSocket, const std::string& message);
    void startDisconnectTimer();
    void stopDisconnectTimer();

    MessageType getMessageType(const std::string& message);

    asio::io_context io_context_;
    asio::ip::tcp::acceptor acceptor_;
    unsigned short port_;
    std::vector<std::shared_ptr<asio::ip::tcp::socket>> userSockets_;
    asio::streambuf receiveBuffer_;
    std::unique_ptr<CommandHandler> commandHandler_;
};
