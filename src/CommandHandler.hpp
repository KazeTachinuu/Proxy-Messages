#ifndef COMMAND_HANDLER_HPP
#define COMMAND_HANDLER_HPP

#include <string>
#include <functional>
#include <unordered_map>
#include <memory>
#include <boost/asio.hpp> // Include boost::asio here

class ProxyServer; // Forward declaration

class CommandHandler
{
public:
    using CommandFunction = std::function<void(const std::shared_ptr<boost::asio::ip::tcp::socket>&, const std::string&)>;

    CommandHandler(ProxyServer& proxyServer);

    void handleCommand(const std::shared_ptr<boost::asio::ip::tcp::socket>& userSocket, const std::string& command);

private:
    void registerCommand(const std::string& command, CommandFunction handler);

    void handleGetUserConnected(const std::shared_ptr<boost::asio::ip::tcp::socket>& userSocket, const std::string& command);
    void handleEchoReply(const std::shared_ptr<boost::asio::ip::tcp::socket>& userSocket, const std::string& command);

    ProxyServer& proxyServer_;
    std::unordered_map<std::string, CommandFunction> commandHandlers_;
};

#endif // COMMAND_HANDLER_HPP
