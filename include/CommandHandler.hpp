#pragma once

#include <functional>
#include <map>
#include <memory>
#include <string>
#include <boost/asio.hpp>

class ProxyServer; // Forward declaration

class CommandHandler
{
public:
    using CommandFunction = std::function<void(const std::shared_ptr<boost::asio::ip::tcp::socket>&, const std::string&)>;

    explicit CommandHandler(ProxyServer& proxyServer);

    // Register a command and its handler function
    void registerCommand(const std::string& command, CommandFunction handler);

    // Handle a command received from a user
    void handleCommand(const std::shared_ptr<boost::asio::ip::tcp::socket>& userSocket, const std::string& command);

private:
    // Command handlers
    void handleGetUserConnected(const std::shared_ptr<boost::asio::ip::tcp::socket>& userSocket, const std::string& command);
    void handleEchoReply(const std::shared_ptr<boost::asio::ip::tcp::socket>& userSocket, const std::string& command);

private:
    ProxyServer& proxyServer_;
    std::map<std::string, CommandFunction> commandHandlers_;
};

